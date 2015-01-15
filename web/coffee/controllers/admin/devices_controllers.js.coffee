model = Model("/api/devices/:id")
class Index
  constructor: (@ctx) ->
    @devices = []
    model.index().done (data) =>
      @devices = data
      @ctx.app.refresh()

app.addController 'admin_devices#index', Index



class Form
  constructor: (@ctx) ->
    @device = {specifications: {} }
    return unless @ctx.params.id

    model.get(id: @ctx.params.id).done (data) =>
      @device = data
      @ctx.app.refresh()

  submit: ->
    if @device.id
      promise = model.put id: @device.id, @device
    else
      promise = model.post {}, @device
    promise.done =>
      @ctx.app.visit '/admin/devices'


app.addController 'admin_devices#new', Form
app.addController 'admin_devices#edit', Form
