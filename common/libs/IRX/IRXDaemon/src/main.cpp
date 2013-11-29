/*
**      IridiumFrameworks
**
**      Original Copyright (C) 2013 - 2013 HORIGUCHI Junshi.
**                                          http://iridium.jp/
**                                          zap00365@nifty.com
**      Portions Copyright (C) <year> <author>
**                                          <website>
**                                          <e-mail>
**      Version     POSIX / C++03
**      Website     http://iridium.jp/
**      E-mail      zap00365@nifty.com
**
**      This source code is for Xcode.
**      Xcode 4.6.2 (Apple LLVM compiler 4.2, LLVM GCC 4.2)
**
**      main.cpp
**
**      ------------------------------------------------------------------------
**
**      The MIT License (MIT)
**
**      Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
**      associated documentation files (the "Software"), to deal in the Software without restriction,
**      including without limitation the rights to use, copy, modify, merge, publish, distribute,
**      sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
**      furnished to do so, subject to the following conditions:
**      The above copyright notice and this permission notice shall be included in all copies or
**      substantial portions of the Software.
**      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
**      BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
**      IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
**      WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
**      OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
**      以下に定める条件に従い、本ソフトウェアおよび関連文書のファイル（以下「ソフトウェア」）の複製を
**      取得するすべての人に対し、ソフトウェアを無制限に扱うことを無償で許可します。
**      これには、ソフトウェアの複製を使用、複写、変更、結合、掲載、頒布、サブライセンス、および、または販売する権利、
**      およびソフトウェアを提供する相手に同じことを許可する権利も無制限に含まれます。
**      上記の著作権表示および本許諾表示を、ソフトウェアのすべての複製または重要な部分に記載するものとします。
**      ソフトウェアは「現状のまま」で、明示であるか暗黙であるかを問わず、何らの保証もなく提供されます。
**      ここでいう保証とは、商品性、特定の目的への適合性、および権利非侵害についての保証も含みますが、それに限定されるものではありません。
**      作者または著作権者は、契約行為、不法行為、またはそれ以外であろうと、ソフトウェアに起因または関連し、
**      あるいはソフトウェアの使用またはその他の扱いによって生じる一切の請求、損害、その他の義務について何らの責任も負わないものとします。
*/

#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/param.h>
#include <libutil.h>
#include "IRXDaemon.h"

enum OperationEnum {
    OPERATION_UNKNOWN,
    OPERATION_START,
    OPERATION_RESTART,
    OPERATION_STOP
};
enum ModeEnum {
    MODE_UNKNOWN,
    MODE_DAEMON,
    MODE_APPLICATION
};

static  void                            onSigHUP        (int param);
static  void                            onSigINT        (int param);
static  void                            onSigTERM       (int param);

static  sig_atomic_t volatile           g_quit;

int main(int argc, char const* argv[])
{
    ir::IRXDaemon* instance;
    OperationEnum operation;
    ModeEnum mode;
    struct pidfh* pfh;
    pid_t pid;
    bool daemonize;
    int i;
    int result(EXIT_SUCCESS);
    
    if ((instance = ir::IRXDaemon::_new()) != NULL) {
        operation = OPERATION_UNKNOWN;
        mode = MODE_UNKNOWN;
        if (argc > 1) {
            if ((result = instance->usage(argc, argv)) == EXIT_SUCCESS) {
                for (i = 1; i < argc; ++i) {
                    if (strcmp(argv[i], "start") == 0) {
                        operation = OPERATION_START;
                    }
                    else if (strcmp(argv[i], "restart") == 0) {
                        operation = OPERATION_RESTART;
                    }
                    else if (strcmp(argv[i], "stop") == 0) {
                        operation = OPERATION_STOP;
                    }
                    else if (strcmp(argv[i], "--daemon") == 0) {
                        mode = MODE_DAEMON;
                    }
                    else if (strcmp(argv[i], "--application") == 0) {
                        mode = MODE_APPLICATION;
                    }
                }
            }
        }
        if (result == EXIT_SUCCESS) {
            if (operation == OPERATION_UNKNOWN) {
                operation = OPERATION_START;
            }
            if (mode == MODE_UNKNOWN) {
                mode = MODE_DAEMON;
            }
            g_quit = false;
            signal(SIGHUP, onSigHUP);
            signal(SIGINT, onSigINT);
            signal(SIGTERM, onSigTERM);
            signal(SIGCHLD, SIG_IGN);
            switch (operation) {
                case OPERATION_RESTART:
                case OPERATION_STOP:
                    if ((pfh = pidfile_open(NULL, 0600, &pid)) != NULL) {
                        instance->log(LOG_NOTICE, "already stopped");
                        pidfile_remove(pfh);
                    }
                    else {
                        switch (errno) {
                            case EEXIST:
                                if (kill(pid, SIGTERM) == 0) {
                                    instance->log(LOG_NOTICE, "stopping pid = %d...", pid);
                                    while (true) {
                                        if (g_quit) {
                                            result = EXIT_FAILURE;
                                            break;
                                        }
                                        if (kill(pid, 0) != 0) {
                                            instance->log(LOG_NOTICE, "stopping done");
                                            break;
                                        }
                                        sleep(1);
                                    }
                                }
                                else {
                                    instance->log(LOG_ERR, "can't stop pid = %d [%m]", pid);
                                    result = EX_OSERR;
                                }
                                break;
                            case EACCES:
                                instance->log(LOG_ERR, "run as super user");
                                result = EX_OSERR;
                                break;
                            default:
                                instance->log(LOG_ERR, "can't stop [%m]");
                                result = EX_OSERR;
                                break;
                        }
                    }
                    break;
                default:
                    break;
            }
            if (result == EXIT_SUCCESS) {
                switch (operation) {
                    case OPERATION_START:
                    case OPERATION_RESTART:
                        if ((pfh = pidfile_open(NULL, 0600, &pid)) != NULL) {
                            instance->log(LOG_NOTICE, "starting pid = %d...", getpid());
                            daemonize = true;
                            if (mode == MODE_APPLICATION) {
                                daemonize = false;
                            }
                            #ifdef __APPLE__
                            if (getppid() == 1) {
                                daemonize = false;
                            }
                            #endif
                            if (daemonize) {
                                if (daemon(0, 0) != 0) {
                                    instance->log(LOG_ERR, "can't daemonize pid = %d [%m]", getpid());
                                    result = EX_OSERR;
                                }
                            }
                            if (result == EXIT_SUCCESS) {
                                instance->log(LOG_NOTICE, "changing pid = %d...", getpid());
                                pidfile_write(pfh);
                                if ((result = instance->initialize()) == EXIT_SUCCESS) {
                                    instance->log(LOG_NOTICE, "starting done");
                                    while (true) {
                                        if (g_quit) {
                                            result = EXIT_FAILURE;
                                            break;
                                        }
                                        instance->loop();
                                    }
                                }
                                else {
                                    instance->log(LOG_ERR, "can't start pid = %d [%d]", getpid(), result);
                                }
                                instance->terminate();
                            }
                            pidfile_remove(pfh);
                        }
                        else {
                            switch (errno) {
                                case EEXIST:
                                    instance->log(LOG_ERR, "already started pid = %d", pid);
                                    result = EX_OSERR;
                                    break;
                                case EACCES:
                                    instance->log(LOG_ERR, "run as super user");
                                    result = EX_OSERR;
                                    break;
                                default:
                                    instance->log(LOG_ERR, "can't start [%m]");
                                    result = EX_OSERR;
                                    break;
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        ir::IRXDaemon::_delete(instance);
    }
    else {
        result = EXIT_FAILURE;
    }
    return result;
}

static void onSigHUP(int param)
{
    g_quit = true;
    return;
}

static void onSigINT(int param)
{
    g_quit = true;
    return;
}

static void onSigTERM(int param)
{
    g_quit = true;
    return;
}
