package org.elastos.sdk.elephantwallet.contact.internal;

import android.util.Log;

import org.elastos.sdk.elephantwallet.contact.Contact;
import org.elastos.tools.crosspl.CrossBase;
import org.elastos.tools.crosspl.annotation.CrossClass;
import org.elastos.tools.crosspl.annotation.CrossInterface;

import java.nio.ByteBuffer;

@CrossClass
public abstract class ContactDataListener extends CrossBase {
    public abstract void onResult(String humanCode, ContactChannel channelType,
                                   String dataId, int errCode);
    public abstract int onReadData(String humanCode, ContactChannel channelType,
                                   String dataId, long offset, ByteBuffer data);
    public abstract int onWriteData(String humanCode, ContactChannel channelType,
                                    String dataId, long offset, byte[] data);

    protected ContactDataListener() {
        super(ContactDataListener.class.getName(), 0);
    }

    @CrossInterface
    private void onResult(String humanCode, int channelType,
                              String dataId, int errCode) {
        onResult(humanCode, ContactChannel.valueOf(channelType),
                dataId, errCode);
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
