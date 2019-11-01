package org.elastos.sdk.elephantwallet.contact.internal;

import android.util.Log;

import org.elastos.sdk.elephantwallet.contact.Contact;
import org.elastos.tools.crosspl.CrossBase;
import org.elastos.tools.crosspl.annotation.CrossClass;
import org.elastos.tools.crosspl.annotation.CrossInterface;

import java.nio.ByteBuffer;

@CrossClass
public abstract class ContactDataListener extends CrossBase {
    public enum Status {
        Unknown      (1000),
        Initialized  (1001),
        Destroyed    (1002),
        Connecting   (1003),
        Connected    (1004),
        Transmitting (1005),
        Closed       (1006),
        Failed       (1007);

        public static Status valueOf(int id) {
            Status[] values = Status.values();
            for(int idx = 0; idx < values.length; idx++) {
                if(values[idx].id == id) {
                    return values[idx];
                }
            }
            return null;
        }

        private Status(int id){
            this.id = id;
        }
        private int id;
    }

    public abstract void onNotify(String humanCode, ContactChannel channelType,
                                   String dataId, Status status);
    public abstract int onReadData(String humanCode, ContactChannel channelType,
                                   String dataId, long offset, ByteBuffer data);
    public abstract int onWriteData(String humanCode, ContactChannel channelType,
                                    String dataId, long offset, byte[] data);

    protected ContactDataListener() {
        super(ContactDataListener.class.getName(), 0);
    }

    @CrossInterface
    private void onNotify(String humanCode, int channelType,
                              String dataId, int status) {
        onNotify(humanCode, ContactChannel.valueOf(channelType),
                dataId, Status.valueOf(status));
    }

    @CrossInterface
    private byte[] onReadData(String humanCode, int channelType,
                              String dataId, long offset) {
//        Contact.Debug.DumpLocalRefTables();
        ByteBuffer dataBuf = ByteBuffer.allocate(1024);
        int ret = onReadData(humanCode, ContactChannel.valueOf(channelType),
                             dataId, offset,
                             dataBuf);
        if(ret < 0) {
            return null;
        }

        int size = ret;
        byte[] data = new byte[size];
        dataBuf.get(data, 0, size);

        return data;
    }

    @CrossInterface
    private int onWriteData(String humanCode, int channelType,
                            String dataId, long offset, byte[] data) {
//        Contact.Debug.DumpLocalRefTables();
        int ret = onWriteData(humanCode, ContactChannel.valueOf(channelType),
                              dataId, offset,
                              data);

        return ret;
    }
}
