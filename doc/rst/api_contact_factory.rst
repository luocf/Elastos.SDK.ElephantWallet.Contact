Contact.Factory
================

.. cpp:class:: Contact.Factory

  Static class, the contact factory of SDK.

SetLogLevel
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: static void SetLogLevel(int level)

  Set sdk log level.

  **Parameter**
    :[in] level: log level, same as android.util.Log.

SetDeviceId
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: static void SetDeviceId(String devId)

  Set device id.

  **Parameter**
    :[in] devId: unique device id, such as ANDROID_ID.

SetLocalDataDir
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: static int SetLocalDataDir(String dir)

  Set local dir where contact data saved.

  **Parameter**
    :[in] dir: directory absolute path.

  **Return**
    =0 if succeeded, or <0 if failed.


Create
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: static Contact Create() {

  Create a contact instance.

  **Return**
    Contact object if succeeded, or null if failed.

