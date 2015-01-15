model = Model("/api/users/:id")
class Index
  constructor: (@ctx) ->
    @users = []
    model.index().done (data) =>
      @users = data
      @ctx.app.refresh()

app.addController 'admin_users#index', Index



class Form
  constructor: (@ctx) ->
    @user = {}
    return unless @ctx.params.id

    model.get(id: @ctx.params.id).done (data) =>
      @user = data
      @ctx.app.refresh()

  submit: ->
    if @user.id
      promise = model.put id: @user.id, @user
    else
      promise = model.post {}, @user
    promise.done =>
      @ctx.app.visit '/admin/users'


app.addController 'admin_users#new', Form
app.addController 'admin_users#edit', Form
