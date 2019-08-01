Getting started with the contact SDK
########################################

.. toctree::
  :maxdepth: 4

.. api:

APIs
---------

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

Contact
================
.. cpp:class:: Contact
  The contact class of SDK.

Contact
~~~~~~~~~~~~~~~~~~~
.. cpp:function:: Contact()
  Constructor of the class.

setListener
~~~~~~~~~~~~~~~~~~~
.. cpp:function:: void setListener(Contact.Listener listener)
  Set contact listener to instance.
  **Parameter**
    :[in] listener: listener to process all callback.

getUserInfo
~~~~~~~~~~~~~~~~~~~
.. cpp:function:: Contact.UserInfo getUserInfo()
  Get contact user info.
  **Return**
    User info object if succeeded, or null if failed.

listFriendInfo
~~~~~~~~~~~~~~~~~~~
.. cpp:function:: List<Contact.FriendInfo> listFriendInfo()
  Get contact friend info list.
  **Return**
    Friend info list if succeeded, or null if failed.

listFriendCode
~~~~~~~~~~~~~~~~~~~
.. cpp:function:: List<String> listFriendCode()
  Get contact friend key code list.
  **Return**
    Friend code list if succeeded, or null if failed.

getStatus
~~~~~~~~~~~~~~~~~~~
.. cpp:function:: ContactStatus getStatus(String humanCode)
  Get contact user or friend status.
  **Parameter**
    :[in] humanCode: user or friend key code.
  **Return**
    status if succeeded, or null if failed.
