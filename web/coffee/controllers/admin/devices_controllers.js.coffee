class Index
  layout: 'admin'
  constructor: (@ctx) ->
    @devices = []
    @model = app.model 'device'
    @model.index().done (data) =>
      @devices = data
      @ctx.app.refresh()

app.addController 'admin_devices#index', Index



class Form
  layout: 'admin'
  constructor: (@ctx) ->
    @device = {specifications: {} }
    return unless @ctx.params.id
    @model = app.model 'device'
    @model.get(id: @ctx.params.id).done (data) =>
      @device = data
      @ctx.app.refresh()

  submit: ->
    if @device.id
      promise = @model.put id: @device.id, @device
    else
      promise = @model.post {}, @device
    promise.done =>
      @ctx.app.visit '/admin/devices'


app.addController 'admin_devices#new', Form
app.addController 'admin_devices#edit', Form
