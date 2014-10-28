class Domoio
  constructor: ->
    @eventsListeners = {}
    @allEventsListeners = []

  version: '0.1';
  emit: (channel, event) ->
    for l in @allEventsListeners
      l.call @, event

  onAll: (callback) -> @allEventsListeners.push callback

  on: (event, callback) ->



module.exports = new Domoio();
