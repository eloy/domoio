tmpl = '''
<div class="device">
  <h4>
    {{device.label}}
    <small if="!device.connected">Disconnected</small>
  </h4>
  <div class="ports btn-group" repeat="port in device.ports">
    <button class="btn btn-sm" cclass="'btn-success': port.value, 'btn-default': !port.value" click="togglePortState(port)">
      {{port.name}}
      <span if="device.connected">{{port.value.toString()}}</span>
    </button>
  </div>
</div>
'''

class DeviceComponent
  @template: tmpl
  build: (ctx, meta) ->
    @device = ctx.scope.device
    @model = Model("/api/devices/:device_id/ports/:id/:action")
    window.model = @model

  togglePortState: (port) =>
    port.value = !port.value
    params = device_id: @device.id, id: port.id, action: 'set'
    @model.put params, value: port.value
    return true

app.addComponent 'device', DeviceComponent
