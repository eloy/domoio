class DevicesManager
  constructor: ->
    @devices = []
    DevicesModel = app.model 'device'
    DevicesModel.index().done (d) =>
      @devices = d
      window.devices = d

    ws = new ReconnectingWebSocket("ws://#{location.host}/api/events")
    ws.onerror = (error) ->
      console.log "WS ERROR"
      console.log error

    ws.onopen = () ->
      console.log "WS Connected"

    ws.onmessage = (ev) =>
      data = JSON.parse ev.data
      console.log data
      # Port Set
      if data.type == "port_set"
        d = @device(data.device_id)
        for p in d.ports when p.id == data.port_id
          p.value = data.value
          app.refresh()

      else if data.type == "device_connected"
        console.log "Connected"
        d = @device(data.device_id)
        d.connected = true

      else if data.type == "device_disconnected"
        console.log "Disconnected"
        d = @device(data.device_id)
        d.connected = false


  device: (id) ->
    return d for d in @devices when d.id == id
    return false


app.DevicesManager = DevicesManager
