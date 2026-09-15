#ifndef MPR_ASSERT_H
#define MPR_ASSERT_H

#define GF_ASSERT(expr) ((expr) ? (void)0 : ErrorHandling_AssertFail())

/*
 * void ErrorHandling_AssertFail(void)
 *
 * Called when an assertion fails.
 * Do not use this directly.
 * Instead, use the GF_ASSERT macro defined above.
 */
void ErrorHandling_AssertFail(void);

#endif /* MPR_ASSERT_H */
