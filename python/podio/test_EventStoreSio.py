#!/usr/bin/env python3
"""Python unit tests for the SIO backend"""

import unittest
import os

from test_utils import SKIP_SIO_TESTS  # pylint: disable=import-error
from test_EventStore import EventStoreBaseTestCaseMixin  # pylint: disable=import-error

from podio.EventStore import EventStore


@unittest.skipIf(SKIP_SIO_TESTS, "no SIO support")
class EventStoreSioTestCase(EventStoreBaseTestCaseMixin, unittest.TestCase):
  """Test cases for root input files"""
  def setUp(self):
    """setup an EventStore reading an SIO file"""
    self.filename = 'sio_io/example.sio'
    self.assertTrue(os.path.isfile(self.filename))
    self.store = EventStore([self.filename])

  def test_no_file(self):
    '''Test that non-accessible files are gracefully handled.'''
    with self.assertRaises(ValueError):
      self.store = EventStore('foo.sio')


if __name__ == '__main__':
  # NOTE: These tests are really not intended to be run directly as they depend
  # on quite some environment setup as well as externally produced inputs.
  # See the CMakeLists.txt file in the tests folder for the specifics of that
  # environment and the inputs
  unittest.main()
