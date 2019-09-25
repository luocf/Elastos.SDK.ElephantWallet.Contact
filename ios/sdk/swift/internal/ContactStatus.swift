
public enum ContactStatus: Int, Codable {
  case Invalid = 0x0
  case WaitForAccept = 0x1
  case Offline = 0x2
  case Online = 0x4
  case Removed = 0x8
}
