#include <check.h>
#include "../src/instructions.h"

START_TEST (test_lc3_basic)
{
    Register* reg = create_register();
    Memory* mem = create_memory();

    uint16_t instr = 0b0;
    handle_instruction(reg, mem, instr);

    free_memory(mem);
    free_register(reg);
}
END_TEST

Suite * suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("lc3");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_lc3_basic);
    suite_add_tcase(s, tc_core);

    return s;
}

int main() {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}