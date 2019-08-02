Contact.Message
================

.. cpp:class:: Contact.Message

  The contact message class of SDK.


type
~~~~~~~~~~~~~~~~~~~

.. cpp:member:: Type type

  Message type.


data
~~~~~~~~~~~~~~~~~~~

.. cpp:member:: byte[] data

  Message data.


cryptoAlgorithm
~~~~~~~~~~~~~~~~~~~

.. cpp:member:: String cryptoAlgorithm

  Message crypto algorithm.


timestamp
~~~~~~~~~~~~~~~~~~~

.. cpp:member:: long timestamp

  Message timestamp.

Contact.Message.Type
================

.. cpp:enum:: Contact.Message.Type

  Message type.

  .. cpp:enumerator:: Empty = 0x00000000

  .. cpp:enumerator:: MsgText = 0x00000001

  .. cpp:enumerator:: MsgAudio = 0x00000002

  .. cpp:enumerator:: MsgTransfer = 0x00000004

