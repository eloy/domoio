class SiteController
  constructor: (@ctx) ->
    @_devices = []
    DevicesModel = app.model 'device'
    DevicesModel.index().done (selected) =>
      for d in selected
        @_devices.push app.devicesManager.device d.id
      @ctx.changed()

  devices: ->
    return @_devices

  test: ->
    console.log "FOOO"

app.addController 'site_controller', SiteController
