import Darwin
import Foundation

func main()
{
  print("CrossPL: anno start...")
  
  if(CommandLine.argc < 6) {
    print("Bad Arguments");
    exit(1)
  }

  let resDir = NSURL.fileURL(withPath: CommandLine.arguments[1])
  let productName = CommandLine.arguments[2]
  let bundleId = CommandLine.arguments[3]
  let swiftSrcDir = NSURL.fileURL(withPath: CommandLine.arguments[4])
  let crossplDir = NSURL.fileURL(withPath: CommandLine.arguments[5])

  
  CrossTmplUtils.resourcesDir = resDir
  
  if(FileManager.default.fileExists(atPath: swiftSrcDir.path) == false) {
    print("[\(swiftSrcDir.path)] not exists.")
    exit(1)
  }

  let enumerator = FileManager.default.enumerator(atPath: swiftSrcDir.path)
  let filePaths = enumerator?.allObjects as! [String]
  var swiftSrcList = filePaths.filter({$0.contains(".swift")})
  for idx in swiftSrcList.indices {
    swiftSrcList[idx] = swiftSrcDir.appendingPathComponent(swiftSrcList[idx]).path
  }
  
  print("Found swift source file:")
  for src in swiftSrcList {
    print("  \(src)")
  }
  
  print("Make proxy target dir: \(crossplDir.path)")
  do {
    try FileManager.default.createDirectory(at: crossplDir, withIntermediateDirectories: true)
    try FileManager.default.createDirectory(at: crossplDir.appendingPathComponent("proxy"), withIntermediateDirectories: true)
  } catch {
    print("Failed to create proxy target dir: \(crossplDir.path)")
    exit(1)
  }
  
  let ret = CrossClassAnnoProcessor.process(swiftSrcList: swiftSrcList, crossplDir: crossplDir, productName: productName, bundleId: bundleId)
  if(ret == false) {
    print("Failed to process crosspl anno.")
    exit(1)
  }
}

main()
