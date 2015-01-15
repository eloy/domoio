Request = (method, url, data) ->
  request = (method, url, data, callback, errorCallback) ->
    r = new XMLHttpRequest()
    r.onreadystatechange = ->
      if r.readyState == 4
        if r.status == 200
          callback(r.responseText)
        else
          error = new Error('Server responded with a status of ' + r.status)
          errorCallback error

    # if ModelConfig.server
    #   url = "#{ModelConfig.server}#{url}"

    r.open method, url

    # Send data as JSON encoded
    if method != 'GET' && data
      r.setRequestHeader('Content-type','application/json; charset=utf-8')
      data = JSON.stringify(data)

    r.send(data)

  new Promise (fulfill, reject) ->
    success = (resp) -> fulfill(resp)
    error = (error) -> reject(error)
    request method, url, data, success, error


bindResult = (promise, res) ->
  if res instanceof Array
    Array::push.apply(promise, res)
  else
    promise[k] = v for k,v of res


window.Model = (base) ->
  buildUrl = (base, params) ->
    url = base.replace /:([\w_-]+)/g, (match, capture) ->
      if params[capture] == undefined then '' else params[capture]

    url = url.replace /\/\//g, '/'
    url = url.slice(0, -1) if url.substr(-1,1) == '/'
    return url

  index: (params={}) -> @get(params)
  get: (params={}) ->
    url = buildUrl base, params
    p = Request('GET', url).then(JSON.parse).then (res) ->
      bindResult p, res
      return res

  put: (params={}, data) ->
    url = buildUrl base, params
    p = Request('PUT', url, data).then(JSON.parse).then (res) ->
      bindResult p, res
      return res

  post: (params={}, data) ->
    url = buildUrl base, params
    p = Request('POST', url, data).then(JSON.parse).then (res) ->
      bindResult p, res
      return res
