
// TOP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct Object_Bits{
    int gun;     //   1
    int hunter;  //   4
    int washer;  //   8
    int monkey;  //  16
    int wolf;    //  32
    int goat;    //  64
    int cabbage; // 128
    int player;  // 512
};

enum{
    This,
    Other,
    ThisHidden,
    OtherHidden,
};

static const int STATE_COUNT = 4*2*2*2*2*2*4*2;

Object_Bits
integer_to_bits(int i){
    Object_Bits b = {0};
#define Extract(N,n) N = i%n; i /= n
    Extract(b.gun, 4);
    Extract(b.hunter, 2);
    Extract(b.washer, 2);
    Extract(b.monkey, 2);
    Extract(b.wolf, 2);
    Extract(b.goat, 2);
    Extract(b.cabbage, 4);
    Extract(b.player, 2);
#undef Extract
    return(b);
}

int
bits_to_integer(Object_Bits b){
    int i = 0;
    int a = 1;
#define Encode(N,n) i = i + N*a; a *= n
    Encode(b.gun, 4);
    Encode(b.hunter, 2);
    Encode(b.washer, 2);
    Encode(b.monkey, 2);
    Encode(b.wolf, 2);
    Encode(b.goat, 2);
    Encode(b.cabbage, 4);
    Encode(b.player, 2);
#undef Encode
    return(i);
}

int
config_is_legal(Object_Bits b){
    int result = true;
#define Range(n, m) result = (result && (n >= 0) && (n < m))
    Range(b.gun, 4);
    Range(b.hunter, 2);
    Range(b.washer, 2);
    Range(b.monkey, 2);
    Range(b.wolf, 2);
    Range(b.goat, 2);
    Range(b.cabbage, 4);
    Range(b.player, 2);
#undef Range
    
    if (b.gun == ThisHidden && b.washer != This){
        result = false;
    }
    if (b.cabbage == ThisHidden && b.washer != This){
        result = false;
    }
    if (b.gun == OtherHidden && b.washer != Other){
        result = false;
    }
    if (b.cabbage == OtherHidden && b.washer != Other){
        result = false;
    }
    
    if (b.gun >= ThisHidden && b.cabbage >= ThisHidden){
        result = false;
    }
    
    return(result);
}

int
flip(int v){
    switch (v){
        case This:        v = Other; break;
        case Other:       v = This; break;
        case ThisHidden:  v = OtherHidden; break;
        case OtherHidden: v = ThisHidden; break;
    }
    return(v);
}

Object_Bits
flip(Object_Bits b){
    b.gun = flip(b.gun);
    b.hunter = flip(b.hunter);
    b.washer = flip(b.washer);
    b.monkey = flip(b.monkey);
    b.wolf = flip(b.wolf);
    b.goat = flip(b.goat);
    b.cabbage = flip(b.cabbage);
    b.player = flip(b.player);
    return(b);
}

int
config_is_safe_side(Object_Bits b){
    int safe = true;
    
    if (b.player != This){
        if (b.hunter == This && b.gun == This && b.wolf == This){
            safe = false;
        }
        
        if (b.wolf == This && b.goat == This){
            safe = false;
        }
        
        if (b.wolf == This && b.monkey == This && b.washer != This){
            safe = false;
        }
        
        if (b.goat == This && b.cabbage == This){
            safe = false;
        }
        
        if (b.goat == This && b.gun == This && b.hunter != This){
            safe = false;
        }
        
        if (b.monkey == This && b.cabbage == ThisHidden){
            safe = false;
        }
        
        if (b.monkey == This && b.gun == ThisHidden && b.cabbage != This){
            safe = false;
        }
    }
    
    if (b.monkey == This && b.cabbage == This && b.wolf != This){
        safe = false;
    }
    
    return(safe);
}

struct Queue{
    int *memory;
    int read;
    int write;
    int max;
};

Queue
make_queue(int max){
    Queue q = {0};
    q.memory = (int*)malloc(sizeof(int)*max);
    q.max = max;
    return(q);
}

int
data_size(Queue *q){
    int r = q->write - q->read;
    if (r < 0){
        r += q->max;
    }
    return(r);
}

void
push(Queue *q, int v){
    int next_write = (q->write + 1)%(q->max);
    assert(next_write != q->read);
    q->memory[q->write] = v;
    q->write = next_write;
}

int
pop(Queue *q){
    assert(q->read != q->write);
    int r = q->memory[q->read];
    q->read = (q->read + 1)%(q->max);
    return(r);
}

struct Stack{
    int *memory;
    int top;
    int max;
};

Stack
make_stack(int max){
    Stack s = {0};
    s.memory = (int*)malloc(sizeof(int)*max*max);
    s.max = max*max;
    return(s);
}

int
data_size(Stack *s){
    int r = s->top;
    return(r);
}

void
push(Stack *s, int v){
    int next_top = s->top + 1;
    assert(next_top <= s->max);
    s->memory[s->top] = v;
    s->top = next_top;
}

int
pop(Stack *s){
    assert(s->top > 0);
    int r = s->memory[--s->top];
    return(r);
}

struct State_Array{
    int *s;
    int count;
};

int
Unhide(int v){
    if (v == ThisHidden){
        v = This;
    }
    if (v == OtherHidden){
        v = Other;
    }
    return(v);
}

int
Hide(int v){
    if (v == This){
        v = ThisHidden;
    }
    if (v == Other){
        v = OtherHidden;
    }
    return(v);
}

int
UnhideFlip(int v){
    v = Unhide(v);
    if (v == This){
        v = Other;
    }
    else if (v == Other){
        v = This;
    }
    return(v);
}

int
HiddenEq(int a, int b){
    int A = Unhide(a);
    int B = Unhide(b);
    return(A == B);
}

int
IsHidden(int v){
    return(v != Unhide(v));
}

int
NotHidden(int v){
    return(!IsHidden(v));
}

State_Array
get_moves(int s, int *safe_table){
    Object_Bits b = integer_to_bits(s);
    
    State_Array array = {0};
    array.s = (int*)malloc(sizeof(int)*14);
    array.count = 0;
    
    {
        Object_Bits c = b;
        c.player = UnhideFlip(c.player);
        int i = bits_to_integer(c);
        if (safe_table[i]){
            array.s[array.count++] = i;
        }
    }
    
    if (HiddenEq(b.gun, b.player)){
        Object_Bits c = b;
        c.gun = UnhideFlip(c.gun);
        c.player = UnhideFlip(c.player);
        int i = bits_to_integer(c);
        if (safe_table[i]){
            array.s[array.count++] = i;
        }
    }
    
    if (HiddenEq(b.gun, b.player) && b.gun != b.washer){
        Object_Bits c = b;
        c.gun = Hide(UnhideFlip(c.gun));
        c.player = UnhideFlip(c.player);
        int i = bits_to_integer(c);
        if (safe_table[i]){
            array.s[array.count++] = i;
        }
    }
    
    if (b.hunter == b.player && b.gun == b.player){
        Object_Bits c = b;
        c.hunter = UnhideFlip(c.hunter);
        c.player = UnhideFlip(c.player);
        int i = bits_to_integer(c);
        if (safe_table[i]){
            array.s[array.count++] = i;
        }
    }
    
    if (b.washer == b.player && b.hunter == b.player && NotHidden(b.gun) && NotHidden(b.cabbage)){
        Object_Bits c = b;
        c.washer = UnhideFlip(c.washer);
        c.player = UnhideFlip(c.player);
        int i = bits_to_integer(c);
        if (safe_table[i]){
            array.s[array.count++] = i;
        }
    }
    
    if (b.monkey == b.player){
        Object_Bits c = b;
        c.monkey = UnhideFlip(c.monkey);
        c.player = UnhideFlip(c.player);
        int i = bits_to_integer(c);
        if (safe_table[i]){
            array.s[array.count++] = i;
        }
    }
    
    if (b.wolf == b.player){
        Object_Bits c = b;
        c.wolf = UnhideFlip(c.wolf);
        c.player = UnhideFlip(c.player);
        int i = bits_to_integer(c);
        if (safe_table[i]){
            array.s[array.count++] = i;
        }
    }
    
    if (b.goat == b.player){
        Object_Bits c = b;
        c.goat = UnhideFlip(c.goat);
        c.player = UnhideFlip(c.player);
        int i = bits_to_integer(c);
        if (safe_table[i]){
            array.s[array.count++] = i;
        }
    }
    
    if (HiddenEq(b.cabbage, b.player)){
        Object_Bits c = b;
        c.cabbage = UnhideFlip(c.cabbage);
        c.player = UnhideFlip(c.player);
        int i = bits_to_integer(c);
        if (safe_table[i]){
            array.s[array.count++] = i;
        }
    }
    
    if (HiddenEq(b.cabbage, b.player) && b.cabbage != b.washer){
        Object_Bits c = b;
        c.cabbage = Hide(UnhideFlip(c.cabbage));
        c.player = UnhideFlip(c.player);
        int i = bits_to_integer(c);
        if (safe_table[i]){
            array.s[array.count++] = i;
        }
    }
    
    if (IsHidden(b.gun) && HiddenEq(b.gun, b.player)){
        Object_Bits c = b;
        c.gun = Unhide(c.gun);
        int i = bits_to_integer(c);
        if (safe_table[i]){
            array.s[array.count++] = i;
        }
    }
    
    if (IsHidden(b.cabbage) && HiddenEq(b.cabbage, b.player)){
        Object_Bits c = b;
        c.cabbage = Unhide(c.cabbage);
        int i = bits_to_integer(c);
        if (safe_table[i]){
            array.s[array.count++] = i;
        }
    }
    
    if (NotHidden(b.gun) && HiddenEq(b.gun, b.player)){
        Object_Bits c = b;
        c.gun = Hide(c.gun);
        int i = bits_to_integer(c);
        if (safe_table[i]){
            array.s[array.count++] = i;
        }
    }
    
    if (NotHidden(b.cabbage) && HiddenEq(b.cabbage, b.player)){
        Object_Bits c = b;
        c.cabbage = Hide(c.cabbage);
        int i = bits_to_integer(c);
        if (safe_table[i]){
            array.s[array.count++] = i;
        }
    }
    
    return(array);
}

void
print_queue(Queue queue){
    int i = 0;
    for (;data_size(&queue);){
        int v = pop(&queue);
        fprintf(stdout, "%4d,", v);
        ++i;
        if (i == 10){
            i = 0;
            fprintf(stdout, "\n");
        }
    }
    if (i != 0){
        fprintf(stdout, "\n");
    }
}

void
print_bits(Object_Bits b){
    Object_Bits flip_b = flip(b);
    
    if (b.gun == This){
        fprintf(stdout, "G");
    }
    if (b.hunter == This){
        fprintf(stdout, "H");
    }
    if (b.washer == This){
        fprintf(stdout, "w");
    }
    if (b.gun == ThisHidden){
        fprintf(stdout, "g");
    }
    if (b.cabbage == ThisHidden){
        fprintf(stdout, "c");
    }
    if (b.monkey == This){
        fprintf(stdout, "M");
    }
    if (b.wolf == This){
        fprintf(stdout, "W");
    }
    if (b.goat == This){
        fprintf(stdout, "T");
    }
    if (b.cabbage == This){
        fprintf(stdout, "C");
    }
    if (b.player == This){
        fprintf(stdout, "P");
    }
    
    fprintf(stdout, " ~~~~~ ");
    
    if (flip_b.player == This){
        fprintf(stdout, "P");
    }
    if (flip_b.cabbage == This){
        fprintf(stdout, "C");
    }
    if (flip_b.goat == This){
        fprintf(stdout, "T");
    }
    if (flip_b.wolf == This){
        fprintf(stdout, "W");
    }
    if (flip_b.monkey == This){
        fprintf(stdout, "M");
    }
    if (flip_b.washer == This){
        fprintf(stdout, "w");
    }
    if (flip_b.gun == ThisHidden){
        fprintf(stdout, "g");
    }
    if (flip_b.cabbage == ThisHidden){
        fprintf(stdout, "c");
    }
    if (flip_b.hunter == This){
        fprintf(stdout, "H");
    }
    if (flip_b.gun == This){
        fprintf(stdout, "G");
    }
    
    fprintf(stdout, "\n");
}

int main(){
    int safe_table[STATE_COUNT];
    memset(safe_table, 0, sizeof(safe_table));
    
    int show_safety_checks = true;
    int show_skipped_branches = true;
    
    for (int i = 0; i < STATE_COUNT; ++i){
        Object_Bits b = integer_to_bits(i);
        if (show_safety_checks){
            print_bits(b);
        }
        
        if (config_is_legal(b)){
            Object_Bits flip_b = flip(b);
            int left = config_is_safe_side(b);
            int right = config_is_safe_side(flip_b);
            if (left && right){
                safe_table[i] = true;
                if (show_safety_checks){
                    fprintf(stdout, "SAFE\n");
                }
            }
            else{
                if (show_safety_checks){
                    fprintf(stdout, "UNSAFE\n");
                }
            }
        }
        else{
            if (show_safety_checks){
                fprintf(stdout, "ILLEGAL\n");
            }
        }
    }
    
    if (show_safety_checks){
        fprintf(stdout, "\n");
    }
    
    Object_Bits end_bits = {0};
    end_bits.gun = Other;
    end_bits.hunter = Other;
    end_bits.washer = Other;
    end_bits.monkey = Other;
    end_bits.wolf = Other;
    end_bits.goat = Other;
    end_bits.cabbage = Other;
    end_bits.player = Other;
    
    int start = 0;
    int end = bits_to_integer(end_bits);
    
    Object_Bits b = integer_to_bits(end);
    fprintf(stdout, "TARGET:\n");
    print_bits(b);
    
    int fastest_path[STATE_COUNT];
    int come_from[STATE_COUNT];
    for (int i = 0; i < STATE_COUNT; ++i){
        fastest_path[i] = STATE_COUNT;
        come_from[i] = STATE_COUNT;
    }
    fastest_path[0] = 0;
    come_from[0] = 0;
    
    Queue pending = make_queue(STATE_COUNT);
    push(&pending, 0);
    
    for (int count = 0; data_size(&pending) > 0; ++count){
        int s = pop(&pending);
        
        int L = fastest_path[s];
        
        State_Array moves = get_moves(s, safe_table);
        
        fprintf(stdout, "FROM:  %d\n", come_from[s]);
        fprintf(stdout, "L:     %d\n", fastest_path[s]);
        fprintf(stdout, "STATE: %d\n", s);
        Object_Bits bits = integer_to_bits(s);
        print_bits(bits);
        
        fprintf(stdout, "POSSIBLE MOVES:\n");
        for (int i = 0; i < moves.count; ++i){
            int new_s = moves.s[i];
            if (fastest_path[new_s] > L + 1){
                fastest_path[new_s] = L + 1;
                come_from[new_s] = s;
                push(&pending, new_s);
                fprintf(stdout, "   ");
                bits = integer_to_bits(new_s);
                print_bits(bits);
            }
            else{
                if (show_skipped_branches){
                    fprintf(stdout, " * ");
                    bits = integer_to_bits(new_s);
                    print_bits(bits);
                }
            }
        }
        
        free(moves.s);
    }
    
    if (fastest_path[end] < STATE_COUNT){
        fprintf(stdout, "MOVES: %d\n", fastest_path[end]);
        
        int *solution = (int*)malloc(sizeof(int)*(fastest_path[end] + 1));
        int sol_counter = 0;
        solution[sol_counter++] = end;
        
        int j = end;
        for (;;){
            j = come_from[j];
            solution[sol_counter++] = j;
            if (j == 0){
                break;
            }
        }
        
        for (int j = sol_counter - 1; j >= 0; --j){
            Object_Bits b = integer_to_bits(solution[j]);
            print_bits(b);
        }
    }
    else{
        fprintf(stdout, "NO SOLUTION FOUND\n");
    }
    
    return(0);
}

// BOTTOM

