model = Model("/api/console")

class ConsoleIndex
  layout: 'admin'
  constructor: (@ctx) ->
    @sessions = []

  createSession: ->
    model.post().done (data) ->
      console.log data

app.addController 'admin_console#index', ConsoleIndex


# Show
#----------------------------------------------------------------------

class ConsoleShow
  layout: 'admin'
  constructor: (@ctx) ->
    @command = ""

  submit: ->
    console.log "submit: #{@command}"
    model.put({}, cmd: @command).done (data) ->
      console.log data
    @command = ""
    @ctx.changed()

app.addController 'admin_console#show', ConsoleShow
