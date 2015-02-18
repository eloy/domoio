module.exports = (grunt) ->


  # Project configuration.
  grunt.initConfig
    pkg: grunt.file.readJSON("package.json")

    coffee:
      compile:
        options: { bare: false }
        files: {
          "web/public/assets/js/domoio_core.js": ["web/coffee/*.coffee", "web/coffee/**/*.coffee" ]
        }

    concat:
      js:
        src: [ "web/lib/js/*.js", "web/public/assets/js/domoio_core.js"]
        dest: "web/public/assets/js/application.js"
        separator: "\n;\n"
      css:
        src: ["web/public/assets/css/domoio_core.css", "web/lib/css/*.css"]
        dest: "web/public/assets/css/application.css"

    uglify:
      dist:
        files:
          "web/public/assets/js/application.min.js": ["web/public/assets/js/application.js"]



    less:
      dist:
        files:
          "web/public/assets/css/domoio_core.css": ["web/less/*.less", "web/less/**/*.less"]


    watch:
      scripts:
        files: ["web/coffee/**/*.coffee"]
        tasks: ['build']
        options:
          spawn: false

  # Load plugins
  grunt.loadNpmTasks "grunt-contrib-coffee"
  grunt.loadNpmTasks("grunt-contrib-concat")
  grunt.loadNpmTasks("grunt-contrib-uglify")
  grunt.loadNpmTasks("grunt-contrib-less")
  grunt.loadNpmTasks('grunt-contrib-watch')

  # Default task(s).
  grunt.registerTask "build", ["coffee", "less", "concat"]
  grunt.registerTask "dist", ["build", "uglify"]
  grunt.registerTask "default", "build"
  return
