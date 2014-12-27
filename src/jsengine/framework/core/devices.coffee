class Devices
  constructor: (@devices) ->

  all: -> @devices

  find: (id) ->
    return d for d in @devices when d.id == id

module.exports = new Devices(__devices())
