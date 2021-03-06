#ifndef INC_C9751E28A4624047847B02CA250CD42E
#define INC_C9751E28A4624047847B02CA250CD42E

struct Counted {
    int* new_calls;
    int* delete_calls;
    uint32_t x;
};

extern "C" {

inline static struct Counted* Counted_New(Counted c) {
    *c.new_calls += 1;
    return new Counted{c};
}
inline static void Counted_Delete(struct Counted* c) {
    *c->delete_calls += 1;
    delete c;
}

}

#endif /* UUID */
