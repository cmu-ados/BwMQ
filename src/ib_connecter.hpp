/*
    Copyright (c) 2007-2016 Contributors as noted in the AUTHORS file

    This file is part of libzmq, the ZeroMQ core engine in C++.

    libzmq is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    As a special exception, the Contributors give you permission to link
    this library with independent modules to produce an executable,
    regardless of the license terms of these independent modules, and to
    copy and distribute the resulting executable under terms of your choice,
    provided that you also meet, for each linked independent module, the
    terms and conditions of the license of that module. An independent
    module is a module which is not derived from or based on this library.
    If you modify this library, you must extend this exception to your
    version of the library.

    libzmq is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __RDMA_CONNECTER_HPP_INCLUDED__
#define __RDMA_CONNECTER_HPP_INCLUDED__

#include "fd.hpp"
#include "own.hpp"
#include "stdint.hpp"
#include "io_object.hpp"
#include "ib.hpp"

namespace zmq {
class io_thread_t;
class session_base_t;
struct address_t;

class ib_connecter_t : public own_t, public io_object_t {
 public:
  //  If 'delayed_start' is true connecter first waits for a while,
  //  then starts connection process.
  ib_connecter_t(zmq::io_thread_t *io_thread_,
                   zmq::session_base_t *session_,
                   const options_t &options_,
                   address_t *addr_,
                   bool delayed_start_);
  ~ib_connecter_t();

 private:
  //  ID of the timer used to delay the reconnection.
  enum {
    reconnect_timer_id = 1,
    connect_timer_id
  };

  ib_res_t ib_res;
  int setup_ib();
  //  Handlers for incoming commands.
  void process_plug();
  void process_term(int linger_);

  //  Handlers for I/O events.
  void in_event();
  void out_event();
  void timer_event(int id_);

  //  Removes the handle from the poller.
  void rm_handle();

  //  Internal function to start the actual connection establishment.
  void start_connecting();

  //  Internal function to add a connect timer
  void add_connect_timer();

  //  Internal function to add a reconnect timer
  void add_reconnect_timer();

  //  Internal function to return a reconnect backoff delay.
  //  Will modify the current_reconnect_ivl used for next call
  //  Returns the currently used interval
  int get_new_reconnect_ivl();

  //  Open TCP connecting socket. Returns -1 in case of error,
  //  0 if connect was successful immediately. Returns -1 with
  //  EAGAIN errno if async connect was launched.
  int open();

  //  Close the connecting socket.
  void close();

  //  Get the file descriptor of newly created connection. Returns
  //  retired_fd if the connection was unsuccessful.
  fd_t connect();

  //  Tunes a connected socket.
  bool tune_socket(fd_t fd_);

  //  Address to connect to. Owned by session_base_t.
  address_t *const _addr;

  //  Underlying socket.
  fd_t _s;

  //  Handle corresponding to the listening socket, if file descriptor is
  //  registered with the poller, or NULL.
  handle_t _handle;

  //  If true, connecter is waiting a while before trying to connect.
  const bool _delayed_start;

  //  True iff a timer has been started.
  bool _connect_timer_started;
  bool _reconnect_timer_started;

  //  Reference to the session we belong to.
  zmq::session_base_t *const _session;

  //  Current reconnect ivl, updated for backoff strategy
  int _current_reconnect_ivl;

  // String representation of endpoint to connect to
  std::string _endpoint;

  // Socket
  zmq::socket_base_t *const _socket;

  ib_connecter_t(const ib_connecter_t &);
  const ib_connecter_t &operator=(const ib_connecter_t &);
};
}

#endif