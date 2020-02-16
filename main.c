
#include "tcpserver.h"

void test() {
    char* string = "{\"data\": \"kek\"}";
    json_value* val = json_parse(string, 15);
    printf("%s: %s", val->u.object.values->name, val->u.object.values->value->u.string.ptr);
}

void test2() {
    int i;
    MHASH sha512;
    MHASH gost;
    unsigned char* hash_sha512;
    unsigned char* hash_gost;
    char data[] = "hhadhskfhjdkslhfklashdfklhadshfklashdfklhasklfhashdfklhasfkjhasdkdhfkshdf";

    sha512 = mhash_init(MHASH_SHA512);
    if (sha512 == MHASH_FAILED) {exit(1);}
    mhash(sha512, data, strlen(data));
    hash_sha512 = mhash_end(sha512);

    gost = mhash_init(MHASH_GOST);
    if (gost == MHASH_FAILED) exit(1);
    mhash(gost, data, strlen(data));
    hash_gost = mhash_end(gost);

    printf("Hash sha512: ");
    for (i = 0; i < mhash_get_block_size(MHASH_SHA512); i++){
        printf("%.2x", hash_sha512[i]);
    }
    printf("\n");

    printf("Hash gost: ");
    for (i = 0; i < mhash_get_block_size(MHASH_GOST); i++){
        printf("%.2x", hash_gost[i]);
    }
    printf("\n");
}

int main(int argc, char* argv[]) {
//    test2();
//    printf("%d, %d", mhash_get_block_size(MHASH_SHA512), mhash_get_block_size(MHASH_GOST));
    int port = argc==2 ? argv[1] : 8099;
    printf("listening on port %d\n", port);
    init_server(port);
    work();
    return 0;
}
