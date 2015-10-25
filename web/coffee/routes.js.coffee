router = app.router

router.rootOptions partial: '/home/index.html', controller: 'site_controller'

router.route '/admin', partial: '/admin/index.html', controller: 'admin_dashboard', namespace: 'admin', (admin) ->

  # console
  admin.route '/console', partial: '/admin/console/index.html', controller: 'admin_console#index'
  admin.route '/console/:id', partial: '/admin/console/show.html', controller: 'admin_console#show'

  # Devices
  admin.resources 'devices', {}, (member, collection) ->
    member.resources 'ports'

  admin.resources 'users'
