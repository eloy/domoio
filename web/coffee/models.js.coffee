app.addModel 'device', '/api/devices/:id'
app.addModel 'port', "/api/devices/:device_id/ports/:id"
app.addModel 'user', "/api/users/:id"
