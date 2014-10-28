if (typeof String.prototype.startsWith != 'function') {
  // see below for better implementation!
  String.prototype.startsWith = function (str){
    return this.indexOf(str) == 0;
  };
}

/*
 * Include modules and data
 */

var __buildRequire = function(pwd) {
  return function(name) {

    var basePath = function(path) {
      var lastIndex = path.lastIndexOf("/");
      return path.substring(0, lastIndex);
    };

    var baseFilename;

    // User pass a relative path
    if (name.startsWith(".")) {
      baseFilename = pwd + "/" + name;
    }
    else if (name.startsWith("/")) {
      baseFilename = name;
    }
    else {
      baseFilename = __DOMOIO_CORE_ROOT + "/core/" + name;
    }
    var basePath = basePath(baseFilename);


    var script;
    // Check extension

    if (__file_exists(baseFilename + ".coffee")) {
      coffeeSrc = __read_file(baseFilename + ".coffee");
      script = coffee.compile(coffeeSrc);
    }

    else if (__file_exists(baseFilename + ".js")) {
      script = __read_file(baseFilename + ".js");
    }
    else {
      return false;
    }

    // load the module in a isolated environment
    var module = { exports: {} };
    var exports = module.exports;
    var require = __buildRequire(basePath);

    var func = new Function("module", "exports", "require", script);
    func(module, exports, require);
    return module.exports;
  }

}

var require = __buildRequire(__DOMOIO_CORE_ROOT);
var coffee = require("./vendor/coffee-script/lib/coffee-script/coffee-script");
var console = require("console");
var domoio = require("domoio");

var __emit_events = function(channel, serializedData) {
  var data = JSON.parse(serializedData);
  domoio.emit(channel, data);
}


// Include user code
require(__DOMOIO_USER_ROOT + "/application");
