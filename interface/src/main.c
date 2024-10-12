#include <stdlib.h>
#include <stdio.h>

typedef struct Yeller {
    void* data;
    char* (*yell)(struct Yeller y);
    void (*log_yell)(struct Yeller y);
} Yeller;

typedef struct {
    char* name;
} NameYeller;

char* name_yeller_yell(Yeller y) {
    NameYeller* self = (NameYeller*)(y.data);
    char* str = malloc(256);
    sprintf(str, "I'M %s, FUCK YOU!", self->name);
    return str;
}

void name_yeller_log_yell(Yeller y) {
    char* str = name_yeller_yell(y);
    printf("%s\n", str);
    free(str);
}

Yeller new_name_yeller(char* name) {
    NameYeller* self = malloc(sizeof(NameYeller));
    self->name = name;

    return (Yeller){
        .data = (void*)self,
        .yell = name_yeller_yell,
        .log_yell = name_yeller_log_yell,
    };
}

typedef struct {
} RussianYeller;

char* russian_yeller_yell(Yeller y) {
    return "ПОШЕЛ НАХУЙЙ!!";
}

void russian_yeller_log_yell(Yeller y) {
    char* str = russian_yeller_yell(y);
    printf("%s\n", str);
}

Yeller new_russian_yeller() {
    RussianYeller* self = malloc(sizeof(RussianYeller));

    return (Yeller){
        .data = (void*)(self),
        .yell = russian_yeller_yell,
        .log_yell = russian_yeller_log_yell,
    };
}

int main() {
    Yeller russian_yeller = new_russian_yeller();
    Yeller name_yeller = new_name_yeller("Tigran");

    printf("%s\n", russian_yeller.yell(russian_yeller));
    printf("%s\n", name_yeller.yell(name_yeller));

    russian_yeller.log_yell(russian_yeller);
    name_yeller.log_yell(name_yeller);

    return 0;
}
