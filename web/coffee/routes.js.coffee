router = app.router

router.rootOptions layout: '/home/index.html', controller: 'site_controller'

router.route '/admin', layout: '/admin/index', controller: 'admin_index', namespace: 'admin', (admin) ->
  admin.resources 'devices', {}, (member, collection) ->
    member.resources 'ports'

  admin.resources 'users'
