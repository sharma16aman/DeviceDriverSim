#include <stdio.h>
#include "validator.h"

int test_basic(FILE *report);
int test_buffer(FILE *report);
int test_interrupt(FILE *report);
int test_fault(FILE *report);
int test_performance(FILE *report);

static void write_result(FILE *report, const char *name, int passed) {
    fprintf(report, "%s: %s\n", name, passed ? "PASS" : "FAIL");
    printf("%s: %s\n", name, passed ? "PASS" : "FAIL");
}

int validator_run_all(const char *report_path) {
    FILE *report = fopen(report_path, "w");
    if (!report) {
        perror("Could not open test report");
        return 1;
    }

    int failures = 0;

    int rc_basic = test_basic(report);
    write_result(report, "test_basic", rc_basic == 0);
    failures += (rc_basic != 0);

    int rc_buffer = test_buffer(report);
    write_result(report, "test_buffer", rc_buffer == 0);
    failures += (rc_buffer != 0);

    int rc_interrupt = test_interrupt(report);
    write_result(report, "test_interrupt", rc_interrupt == 0);
    failures += (rc_interrupt != 0);

    int rc_fault = test_fault(report);
    write_result(report, "test_fault", rc_fault == 0);
    failures += (rc_fault != 0);

    int rc_perf = test_performance(report);
    write_result(report, "test_performance", rc_perf == 0);
    failures += (rc_perf != 0);

    fprintf(report, "\nTotal failures: %d\n", failures);
    fclose(report);

    return failures == 0 ? 0 : 1;
}
