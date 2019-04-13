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

#include "testutil.hpp"
#include "testutil_unity.hpp"

#include <unity.h>

void setUp() {
  setup_test_context();
}

void tearDown() {
  teardown_test_context();
}

static const char test_endpoint[] = "ipc://@tmp-tester";

void test_roundtrip() {
  void *sb = test_context_socket(ZMQ_DEALER);
  TEST_ASSERT_SUCCESS_ERRNO (zmq_bind(sb, test_endpoint));

  char endpoint[MAX_SOCKET_STRING];
  size_t size = sizeof(endpoint);
  TEST_ASSERT_SUCCESS_ERRNO (
      zmq_getsockopt(sb, ZMQ_LAST_ENDPOINT, endpoint, &size));
  TEST_ASSERT_EQUAL_INT(0, strncmp(endpoint, test_endpoint, size));

  void *sc = test_context_socket(ZMQ_DEALER);
  TEST_ASSERT_SUCCESS_ERRNO (zmq_connect(sc, test_endpoint));

  bounce(sb, sc);

  test_context_socket_close(sc);
  test_context_socket_close(sb);
}

int main(void) {
  setup_test_environment();

  UNITY_BEGIN();
  RUN_TEST(test_roundtrip);
  return UNITY_END();
}
