devices = require 'devices'

class Domoio
  devices: devices

  constructor: ->
    @eventsListeners = {}
    @allEventsListeners = []

  version: '0.1'


  emit: (channel, event) ->
    event.device_id = parseInt(event.device_id)
    event.port_id = parseInt(event.port_id)

    if event.device_id
      event.device = devices.find event.device_id

    for l in @allEventsListeners
      l.call @, event

  onAll: (callback) -> @allEventsListeners.push callback

  on: (event, callback) ->



module.exports = new Domoio();
