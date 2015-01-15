class Select2Directive
  link: (scope, el, meta) ->
    console.log "Installing select2"
    $(el).select2()

app.addDirective 'select2', Select2Directive
