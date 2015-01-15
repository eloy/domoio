window.app = app = new UnicoApp enableRouter: true, templateBasePath: '/assets/tmpl'
window.app.debug = true

$ ->
  app.startRouter()
