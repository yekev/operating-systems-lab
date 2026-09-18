// SPDX-License-Identifier: GPL-2.0-only

#include <linux/completion.h>
#include <linux/err.h>
#include <linux/kmod.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kaiwen Yao");
MODULE_DESCRIPTION("Run a user-mode signal test from a Linux kernel worker");

static char *test_path = "/tmp/csc3150-signal-test";
module_param(test_path, charp, 0444);
MODULE_PARM_DESC(test_path, "Absolute path of the user-mode test executable");

static DECLARE_COMPLETION(test_finished);
static struct task_struct *worker_task;

static int user_helper_worker(void *unused)
{
    char *argv[] = {test_path, NULL};
    static char *envp[] = {
        "HOME=/",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin",
        NULL,
    };
    int status;

    (void)unused;
    pr_info("csc3150_signal: executing %s\n", test_path);
    status = call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
    pr_info("csc3150_signal: helper returned status %d\n", status);
    complete(&test_finished);
    return 0;
}

static int module_worker(void *unused)
{
    struct task_struct *helper_task;

    (void)unused;
    reinit_completion(&test_finished);
    helper_task = kthread_run(user_helper_worker, NULL, "csc3150_helper");
    if (IS_ERR(helper_task)) {
        pr_err("csc3150_signal: failed to start helper thread: %ld\n",
               PTR_ERR(helper_task));
        return PTR_ERR(helper_task);
    }

    pr_info("csc3150_signal: worker pid=%d, helper pid=%d\n",
            current->pid, helper_task->pid);
    wait_for_completion(&test_finished);
    return 0;
}

static int __init program2_init(void)
{
    worker_task = kthread_run(module_worker, NULL, "csc3150_worker");
    if (IS_ERR(worker_task)) {
        int error = PTR_ERR(worker_task);
        worker_task = NULL;
        return error;
    }

    pr_info("csc3150_signal: module loaded\n");
    return 0;
}

static void __exit program2_exit(void)
{
    if (worker_task != NULL) {
        kthread_stop(worker_task);
    }
    pr_info("csc3150_signal: module unloaded\n");
}

module_init(program2_init);
module_exit(program2_exit);
