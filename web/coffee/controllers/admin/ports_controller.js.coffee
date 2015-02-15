class Form
  constructor: (@ctx) ->
    @port = {}
    @output = false
    @digital = false

    @device_id = @ctx.params.device_id
    @model = app.model 'port'
    return unless @device_id

    return unless id = @ctx.params.id
    @model.get(device_id: @device_id, id: id).done (data) =>
      @port = data
      # Need to transform some data
      @digital = if @port.digital then "true" else "false"
      @output = if @port.output then "true" else "false"
      @ctx.app.refresh()

  submit: ->
    @port.id = parseInt(@port.id)
    @port.digital = (@digital == "true")
    @port.output = (@output == "true")

    if @ctx.params.id
      promise = @model.put device_id: @device_id, id: @port.id, @port
    else
      promise = @model.post {device_id: @device_id}, @port

    promise.done (data) =>
      @ctx.app.visit "/admin/devices/#{@device_id}/edit"


app.addController 'admin_ports#new', Form
app.addController 'admin_ports#edit', Form
