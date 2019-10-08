import UIKit

public class Helper {
  public typealias OnListener = (_ result: String?) -> Void

  public static func showImportMnemonic(view: UIViewController, listener: @escaping OnListener) {
    let dialog = UIAlertController(title: nil, message: nil, preferredStyle: .alert)
    dialog.title = "Import Mnemonic"
    
    let rootView = UITextView()
    setDialogContent(dialog, 200, rootView)

    dialog.addAction(UIAlertAction(title: "Import", style: .default, handler: { _ in
      listener(rootView.text)
    }))
    dialog.addAction(UIAlertAction(title: "Cancel", style: .cancel))
    
    showDialog(view, dialog)
  }

  public static func showAddress(view: UIViewController,
                                 listener: @escaping OnListener,
                                 humanCode: [String: String?], presentDevId: String, ext: String?) {
    let dialog = UIAlertController(title: nil, message: nil, preferredStyle: .alert)
    dialog.title = "My Address"
    
    let rootView = makeAddressView(view: view, width: dialog.view.widthAnchor, listener: listener,
                                   humanCode: humanCode, presentDevId: presentDevId, ext: ext)
    setDialogContent(dialog, -1, rootView)

    dialog.addAction(UIAlertAction(title: "Cancel", style: .cancel))
    
    showDialog(view, dialog);
    
//        AlertDialog.Builder builder = new AlertDialog.Builder(context);
//        builder.setTitle("My Address");
//        try {
//            View root = makeAddressView(context, humanCode, presentDevId, ext, listener);
//            builder.setView(root);
//        } catch (Exception e) {
//            String msg = "Failed to show address.";
//            builder.setMessage(msg + e);
//            Log.w(TAG, msg, e);
//        }
//
//        builder.setNegativeButton("Cancel", (dialog, which) -> {
//            dismissDialog();
//        });
//
//        showDialog(builder);
    }
//
//    public static void showSetDetails(Context context, List<String> checkList, String separator, OnListener listener) {
//        AlertDialog.Builder builder = new AlertDialog.Builder(context);
//        builder.setTitle("Set Details");
//
//        RadioGroup radioGrp = new RadioGroup(context);
//        EditText editView = new EditText(context);
//        View root = makeSetDetailView(context, radioGrp, checkList, editView);
//        builder.setView(root);
//
//        builder.setPositiveButton("OK", (dialog, which) -> {
//            int checkedId = radioGrp.getCheckedRadioButtonId();
//            RadioButton checkedBtn = radioGrp.findViewById(checkedId);
//            String key = checkedBtn.getText().toString();
//            String value = editView.getText().toString();
//
//            listener.onResult(key + separator + value);
//        });
//        builder.setNegativeButton("Cancel", (dialog, which) -> {
//            dismissDialog();
//        });
//
//        showDialog(builder);
//    }

  public static func showDetails(view: UIViewController, msg: String) {
    let dialog = UIAlertController(title: nil, message: nil, preferredStyle: .alert)

    dialog.title = "Details"
    let rootView = UITextView()
    rootView.text = msg
    rootView.isEditable = false
    setDialogContent(dialog, -1, rootView)
    dialog.addAction(UIAlertAction(title: "Cancel", style: .cancel))
      
    showDialog(view, dialog);
  }

//    public static void showFriendList(Context context, List<String> friendList, OnListener listener) {
//        ListView listView = new ListView(context);
//        ArrayAdapter<String> adapter = new ArrayAdapter<>(context, android.R.layout.simple_list_item_1, friendList);
//        listView.setAdapter(adapter);
//        listView.setOnItemClickListener((parent, view, position, id) -> {
//            listener.onResult(((TextView)view).getText().toString());
//        });
//
//        AlertDialog.Builder builder = new AlertDialog.Builder(context);
//        builder.setTitle("Friend List");
//        builder.setView(listView);
//        builder.setNegativeButton("Cancel", (dialog, which) -> {
//            dismissDialog();
//        });
//
//        showDialog(builder);
//    }
//
//    public static void showAddFriend(Context context, String friendCode, OnListener listener) {
//        EditText edit = new EditText(context);
//        View root = makeEditView(context, friendCode, edit);
//
//        AlertDialog.Builder builder = new AlertDialog.Builder(context);
//        builder.setTitle("Find Address");
//        builder.setView(root);
//        builder.setNegativeButton("Cancel", (dialog, which) -> {
//            dismissDialog();
//        });
//        builder.setPositiveButton("Add Friend", (dialog, which) -> {
//            listener.onResult(edit.getText().toString());
//        });
//
//        showDialog(builder);
//    }
//
//    public static void showFriendRequest(Context context, String humanCode, String summary, OnListener listener) {
//        AlertDialog.Builder builder = new AlertDialog.Builder(context);
//        builder.setTitle("Friend Request");
//        String msg = new String();
//        msg += "FriendCode:\n  " + humanCode + "\n\n";
//        msg += "Summary:\n  " + summary;
//        builder.setMessage(msg);
//        builder.setPositiveButton("Accept", (dialog, which) -> {
//            listener.onResult(null);
//        });
//        builder.setNegativeButton("Cancel", (dialog, which) -> {
//            dismissDialog();
//        });
//
//        showDialog(builder);
//    }
//
//    public static void showSendMessage(Context context, String friendCode, OnListener listener) {
//        EditText edit = new EditText(context);
//        View root = makeEditView(context, friendCode, edit);
//
//        AlertDialog.Builder builder = new AlertDialog.Builder(context);
//        builder.setTitle("Send Message");
//        builder.setView(root);
//        builder.setNegativeButton("Cancel", (dialog, which) -> {
//            dismissDialog();
//        });
//        builder.setPositiveButton("Send", (dialog, which) -> {
//            listener.onResult(edit.getText().toString());
//        });
//
//        showDialog(builder);
//    }
//
//    public static void scanAddress(MainActivity activity, OnListener listener) {
//        mOnScanListener = listener;
//
//        int hasCameraPermission = ActivityCompat.checkSelfPermission(activity, Manifest.permission.CAMERA);
//        if(hasCameraPermission == PackageManager.PERMISSION_GRANTED) {
//            Intent intent = new Intent(activity, QrCodeActivity.class);
//            activity.startActivityForResult(intent, REQUEST_CODE_QR_SCAN);
//        } else {
//            ActivityCompat.requestPermissions(activity,
//                    new String[]{Manifest.permission.CAMERA},
//                    1);
//        }
//    }
//
//    public static void onRequestPermissionsResult(MainActivity activity, int requestCode, String[] permissions, int[] grantResults) {
//        if (requestCode != 1) {
//            return;
//        }
//
//        for (int idx = 0; idx < permissions.length; idx++) {
//            if(permissions[idx].equals(Manifest.permission.CAMERA) == false) {
//                continue;
//            }
//
//            if (grantResults[idx] == PackageManager.PERMISSION_GRANTED) {
//                Intent intent = new Intent(activity, QrCodeActivity.class);
//                activity.startActivityForResult(intent, REQUEST_CODE_QR_SCAN);
//            }
//        }
//    }
//
//    public static void onActivityResult(MainActivity activity, int requestCode, int resultCode, Intent data) {
//        if(resultCode != Activity.RESULT_OK) {
//            Log.d(TAG,"COULD NOT GET A GOOD RESULT.");
//            if(data == null) {
//                return;
//            }
//            String result = data.getStringExtra("com.blikoon.qrcodescanner.error_decoding_image");
//            if(result == null) {
//                return;
//            }
//
//            activity.showMessage("QR Code could not be scanned.");
//        }
//
//        if(requestCode == REQUEST_CODE_QR_SCAN) {
//            if(data==null)
//                return;
//            //Getting the passed result
//            String result = data.getStringExtra("com.blikoon.qrcodescanner.got_qr_scan_relult");
//            Log.d(TAG,"Scan result:"+ result);
//
//            mOnScanListener.onResult(result);
//            mOnScanListener = null;
//        }
//    }
//
  private static func makeAddressView(view: UIViewController, width: NSLayoutDimension,
                                        listener: @escaping OnListener,
                                        humanCode: [String: String?], presentDevId: String?, ext: String?) -> UIView {
    let txtDevId = UITextView()
    let imgQRCode = UIImageView()
    var radioGrp = [UIStackView]()
    let txtCode =  UITextView()
    let btn =  UIButton()

    txtDevId.isEditable = false
    txtCode.text = "Present DevId: " + (presentDevId ?? "nil")
    
    txtDevId.isEditable = false
    
    let radioGrpChecked: UIControl.TargetClosure = { sender in
      for it in radioGrp {
        (it.subviews.first as! UISwitch).setOn(false, animated: false)
      }
      let radioBtn = sender as! UISwitch
      radioBtn.setOn(true, animated: false)
      
      let mapIdx = radioBtn.tag
      let checkedVal = Array(humanCode)[mapIdx].value
      var showed = checkedVal
      if(mapIdx == humanCode.count - 1) {
        showed! += "\n----------------\n" + ext!;
      }
      
      let bitmap = makeQRCode(value: checkedVal!)
      imgQRCode.image = bitmap
      txtCode.text = showed
    }
    
    for it in humanCode {
      let key = it.key
      let value = it.value!
      
      let radioView = UIStackView()
     
      let radioBtn = UISwitch()
      radioBtn.addTargetClosure(closure: radioGrpChecked)
      radioBtn.tag = radioGrp.count
      radioView.addArrangedSubview(radioBtn)
     
      let radioLabel = UILabel()
      radioLabel.text = key + ": " + value[value.startIndex..<value.index(value.startIndex, offsetBy: 5)] + " ... " + value[value.index(value.endIndex, offsetBy: -5)..<value.endIndex]
      radioView.addArrangedSubview(radioLabel)

      radioGrp.append(radioView)
      if(radioBtn.tag == 0) {
        radioBtn.sendActions(for: .touchUpInside)
      }
    }
    
    btn.setTitle("Details", for: .normal)
    btn.backgroundColor = .blue
    btn.addTargetClosure(closure: { sender in
      listener(nil)
    })
    
    let rootView = UIStackView()
    rootView.axis = .vertical
    rootView.addArrangedSubview(txtDevId)
    rootView.addArrangedSubview(imgQRCode)
    rootView.addArrangedSubview(txtCode);
    for it in radioGrp {
      rootView.addArrangedSubview(it)
    }
    rootView.addArrangedSubview(btn)

    txtDevId.translatesAutoresizingMaskIntoConstraints = false
    txtDevId.heightAnchor.constraint(equalToConstant: 50).isActive = true
    imgQRCode.translatesAutoresizingMaskIntoConstraints = false
    imgQRCode.heightAnchor.constraint(equalTo: imgQRCode.widthAnchor).isActive = true
    txtCode.translatesAutoresizingMaskIntoConstraints = false
    txtCode.heightAnchor.constraint(equalToConstant: 100).isActive = true
    for it in radioGrp {
      it.translatesAutoresizingMaskIntoConstraints = false
      it.widthAnchor.constraint(equalToConstant: 150).isActive = true
      for sub in it.subviews {
        sub.translatesAutoresizingMaskIntoConstraints = false
        sub.heightAnchor.constraint(equalToConstant: 40).isActive = true
      }
    }
    btn.translatesAutoresizingMaskIntoConstraints = false
    btn.heightAnchor.constraint(equalToConstant: 50).isActive = true

    return rootView
  }

//    private static View makeSetDetailView(Context context, RadioGroup radioGrp, List<String> checkList, EditText editView) {
//        TextView txtView = new TextView(context);
//
//        LinearLayout root = new LinearLayout(context);
//        root.setOrientation(LinearLayout.VERTICAL);
//        root.addView(radioGrp);
//        root.addView(txtView);
//        root.addView(editView);
//
//        for(String it: checkList) {
//            RadioButton radiobtn = new RadioButton(context);
//            radiobtn.setText(it);
//            radioGrp.addView(radiobtn);
//            if(radioGrp.getChildCount() == 1) {
//                radiobtn.setChecked(true);
//            }
//        }
//
//        txtView.setText("Value: ");
//        editView.setText("TestUser");
//
//        return root;
//    }
//
//    private static View makeEditView(Context context, String friendCode, EditText edit) {
//        TextView txtCode = new TextView(context);
//        TextView txtMsg = new TextView(context);
//
//        LinearLayout root = new LinearLayout(context);
//        root.setOrientation(LinearLayout.VERTICAL);
//        root.addView(txtCode);
//        root.addView(txtMsg);
//        root.addView(edit);
//
//        txtCode.setText("FriendCode: \n  " + friendCode);
//        txtMsg.setText("Message:");
//        edit.setText("Hello");
//
//        return root;
//    }
//
  private static func makeQRCode(value: String) -> UIImage {
    let data = value.data(using: String.Encoding.ascii)

    let filter = CIFilter(name: "CIQRCodeGenerator")
    filter!.setValue(data, forKey: "inputMessage")
    let transform = CGAffineTransform(scaleX: 3, y: 3)

    let output = (filter!.outputImage?.transformed(by: transform))!
    let ret = UIImage(ciImage: output)
    return ret
  }

  private static func setDialogContent(_ dialog: UIAlertController, _ height: CGFloat, _ contentView: UIView) {
    dialog.view.translatesAutoresizingMaskIntoConstraints = false
    dialog.view.heightAnchor.constraint(equalToConstant: height >= 0.0 ? height : 10000).isActive = true

    dialog.view.addSubview(contentView)
    contentView.translatesAutoresizingMaskIntoConstraints = false
    contentView.leftAnchor.constraint(equalTo: dialog.view.leftAnchor, constant: 10).isActive = true
    contentView.rightAnchor.constraint(equalTo: dialog.view.rightAnchor, constant: -10).isActive = true
    contentView.topAnchor.constraint(equalTo: dialog.view.topAnchor, constant: 45).isActive = true
    contentView.bottomAnchor.constraint(equalTo: dialog.view.bottomAnchor, constant: -45).isActive = true
  }
  
  private static func showDialog(_ view: UIViewController, _ dialog: UIAlertController) {
    dismissDialog()

    mLastDialog = dialog
    view.present(mLastDialog!, animated: false)
  }

  private static func dismissDialog() {
    guard mLastDialog != nil else { return }

    mLastDialog!.dismiss(animated: false)
    mLastDialog = nil
  }

  private static var mLastDialog: UIAlertController? = nil
//    private static OnListener mOnScanListener;
//    private static final int REQUEST_CODE_QR_SCAN = 101;
}

extension UIControl {
  typealias TargetClosure = (UIControl) -> ()

  func addTargetClosure(closure: @escaping TargetClosure) {
    targetClosure = closure
    addTarget(self, action: #selector(UIControl.closureAction), for: .touchUpInside)
  }
  @objc func closureAction() {
      guard let targetClosure = targetClosure else { return }
      targetClosure(self)
  }
  
  private struct AssociatedKeys {
      static var targetClosure = "targetClosure"
  }
  
  private var targetClosure: TargetClosure? {
      get {
          guard let closureWrapper = objc_getAssociatedObject(self, &AssociatedKeys.targetClosure) as? ClosureWrapper else { return nil }
          return closureWrapper.closure
      }
      set(newValue) {
          guard let newValue = newValue else { return }
          objc_setAssociatedObject(self, &AssociatedKeys.targetClosure, ClosureWrapper(newValue), objc_AssociationPolicy.OBJC_ASSOCIATION_RETAIN_NONATOMIC)
      }
  }
  
  class ClosureWrapper: NSObject {
      let closure: TargetClosure
      init(_ closure: @escaping TargetClosure) {
          self.closure = closure
      }
  }
}
