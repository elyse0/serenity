/*
 * Copyright (c) 2020, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "ClientConnection.h"
#include <LibCore/EventLoop.h>
#include <LibCore/LocalServer.h>
#include <LibIPC/ClientConnection.h>
#include <LibMain/Main.h>
#include <LibSystem/Wrappers.h>

ErrorOr<int> serenity_main(Main::Arguments)
{
    Core::EventLoop event_loop;
    TRY(System::pledge("stdio unix rpath recvfd", nullptr));

    auto socket = Core::LocalSocket::take_over_accepted_socket_from_system_server();
    IPC::new_client_connection<LanguageServers::Shell::ClientConnection>(socket.release_nonnull(), 1);
    TRY(System::pledge("stdio rpath recvfd", nullptr));
    TRY(System::unveil("/etc/passwd", "r"));

    return event_loop.exec();
}
