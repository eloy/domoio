console.log "Here I am, user code ready to run"
console.log domoio.version

domoio.onAll (event) ->
  console.log "Event raised #{event.device.label}"
  event.device.ports[0].value = event.value

for device in domoio.devices.all()
  console.log "Device: #{device.id} => #{device.label}"
  for port in device.ports
    console.log "Port: #{port.id} => #{port.value}"
