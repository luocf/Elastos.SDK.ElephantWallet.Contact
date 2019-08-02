Contact.UserInfo
================

.. cpp:class:: Contact.UserInfo : public HumanInfo

  The contact user info class of SDK.
  Inherit from HumanInfo.

Contact.UserInfo
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: Contact.UserInfo()

  Constructor of the class.


getCurrDevCarrierAddr
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: String getCurrDevCarrierAddr()

  Get current device carrier address.

  **Return**
    Carrier address if succeeded, or null if failed.


getCurrDevCarrierId
~~~~~~~~~~~~~~~~~~~

.. cpp:function:: String getCurrDevCarrierId()

  Get current device carrier id.

  **Return**
    Carrier id if succeeded, or null if failed.

HumanInfo
================

.. cpp:class:: HumanInfo

  The contact base class for user and friend info of SDK.


status
~~~~~~~~~~~~~~~~~~~

.. cpp:member:: ContactStatus status

  Online status.


humanCode
~~~~~~~~~~~~~~~~~~~

.. cpp:member:: String humanCode

  Human key code.


chainPubKey
~~~~~~~~~~~~~~~~~~~

.. cpp:member:: String chainPubKey

  Human public key.


did
~~~~~~~~~~~~~~~~~~~

.. cpp:member:: String did

  Human did.


elaAddress
~~~~~~~~~~~~~~~~~~~

.. cpp:member:: String elaAddress

  Human ela address.


nickname
~~~~~~~~~~~~~~~~~~~

.. cpp:member:: String nickname

  Human nickname, NOT implementation.


avatar
~~~~~~~~~~~~~~~~~~~

.. cpp:member:: String avatar

  Human avatar, NOT implementation.


gender
~~~~~~~~~~~~~~~~~~~

.. cpp:member:: String gender

  Human gender, NOT implementation.


description
~~~~~~~~~~~~~~~~~~~

.. cpp:member:: String description

  Human description, NOT implementation.



