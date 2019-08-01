Contact.Listener
================

.. cpp:class:: Contact.Listener

  The contact listener abstract class of SDK.
  You should implement the abstract methods and set it to contact.

onError
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: abstract void onError(int errCode, String errStr, String ext)

  Error callback.

  **Parameter**
    :[in] errCode: error code, all errors are <0.
    :[in] errStr: error description.
    :[in] ext: error extension description, assist with debugging.


onAcquire
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: abstract byte[] onAcquire(AcquireArgs request)

  Request same required data.

  **Parameter**
    :[in] request: type and input arguments of request.

  **Return**
    result of request.


onEvent
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: abstract void onEvent(EventArgs event)

  Event occured to notify.

  **Parameter**
    :[in] event: event type and data.


onReceivedMessage
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: abstract void onReceivedMessage(String humanCode, int channelType, Contact.Message message)

  Message received from friend or other devices.

  **Parameter**
    :[in] humanCode: friend or other devices key code.
    :[in] channelType: channel which message received.
    :[in] message: message object received.

Contact.Listener
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: Contact.Listener()

  Constructor of the class.


