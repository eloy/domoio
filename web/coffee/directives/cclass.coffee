class CClassDirective
  constructor: ->
    @el = undefined
    @opts = undefined

  link: (ctx, el, meta) ->
    @updateCss(ctx, el, meta)

  updateCss: (ctx, el, meta) ->
    opts = ctx.eval "{#{meta.attrs.cclass}}"
    return unless opts

    classes = el.className.split(" ")
    for css, value of opts
      index = classes.indexOf css
      if value && index == -1
        classes.push css
      else if !value && index != -1
        classes.splice index, 1
      el.className = classes.join(" ")

  onRefresh: (ctx, el, meta) ->
    @updateCss(ctx, el, meta)
    return true


app.addDirective 'cclass', CClassDirective
