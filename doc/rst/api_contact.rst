Contact
================

.. cpp:class:: Contact

  The contact class of SDK, create instance by Contact.Factory.Create().

setListener
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: void setListener(Contact.Listener listener)

  Set contact listener to instance.

  **Parameter**
    :[in] listener: listener to process all callback.

start
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: int start()

  Start to run contact instance after created.

  **Return**
    =0 if succeeded, or <0 if failed.

stop
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: int stop()

  Stop to run contact instance after created.

  **Return**
    =0 if succeeded, or <0 if failed.

setUserInfo
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: Contact.UserInfo setUserInfo(UserInfo.Item item, String value)

  Set contact user info, ONLY valid after contact is started.

  **Parameter**
    :[in] item: user info type.
    :[in] value: user info value.

  **Return**
    =0 if succeeded, or <0 if failed.

setIdentifyCode
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: Contact.UserInfo setIdentifyCode(IdentifyCode.Type type, String value)

  Set contact user identify code, ONLY valid after contact is started.

  **Parameter**
    :[in] type: user identify type.
    :[in] value: user identify value.

  **Return**
    =0 if succeeded, or <0 if failed.


getUserInfo
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: Contact.UserInfo getUserInfo()

  Get contact user info, ONLY valid after contact is started.

  **Return**
    User info object if succeeded, or null if failed.


listFriendInfo
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: List<Contact.FriendInfo> listFriendInfo()

  Get contact friend info list, ONLY valid after contact is started.

  **Return**
    Friend info list if succeeded, or null if failed.


listFriendCode
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: List<String> listFriendCode()

  Get contact friend key code list, ONLY valid after contact is started.

  **Return**
    Friend code list if succeeded, or null if failed.


getStatus
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: ContactStatus getStatus(String humanCode)

  Get contact user or friend status, ONLY valid after contact is started.

  **Parameter**
    :[in] humanCode: user or friend key code.

  **Return**
    status if succeeded, or null if failed.


makeMessage
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: Contact.Message makeMessage(ContactMessage.Type type, byte[] data, String cryptoAlgorithm)

  Make message instance.

  **Parameter**
    :[in] type: message type, such as MsgText etc.
    :[in] data: message data.
    :[in] cryptoAlgorithm: data crypto algorithm.

  **Return**
    Message object if succeeded, or null if failed.


makeTextMessage
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: Contact.Message makeTextMessage(String data, String cryptoAlgorithm)

  Make text message instance.

  **Parameter**
    :[in] data: message data.
    :[in] cryptoAlgorithm: data crypto algorithm.

  **Return**
    Message object if succeeded, or null if failed.


sendMessage
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: int sendMessage(String friendCode, ContactChannel channelType, Contact.Message message)

  Send message to friend, ONLY valid after contact is online.

  **Parameter**
    :[in] friendCode: friend key code.
    :[in] channelType: channel which send.
    :[in] message: message object which maked by makeMessage.

  **Return**
    =0 if succeeded, or <0 if failed.


addFriend
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: int addFriend(String friendCode, String summary)

  Add a friend, ONLY valid after contact is online.

  **Parameter**
    :[in] friendCode: friend key code.
    :[in] summary: summary.

  **Return**
    =0 if succeeded, or <0 if failed.


removeFriend
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: int removeFriend(String friendCode)

  Remove a friend, after removed, friend status mask to REMOVED. ONLY valid after contact is online.

  **Parameter**
    :[in] friendCode: friend key code.

  **Return**
    =0 if succeeded, or <0 if failed.


acceptFriend
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: int acceptFriend(String friendCode)

  Accept a friend after received a friend request, ONLY valid after contact is online.

  **Parameter**
    :[in] friendCode: friend key code.

  **Return**
    =0 if succeeded, or <0 if failed.


syncInfoDownloadFromDidChain
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: int syncInfoDownloadFromDidChain()

  Download info from did-chain, ONLY valid after contact is started.

  **Return**
    =0 if succeeded, or <0 if failed.


syncInfoUploadToDidChain
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: int syncInfoUploadToDidChain()

  Upload info to did-chain, ONLY valid after contact is started.

  **Return**
    =0 if succeeded, or <0 if failed.
