// Copyright 2020 Your Name <your_email>

#include <http_server.hpp>

void http_connection::start() {
  read_request();
  check_deadline();
}
void http_connection::read_request() {
  auto self = shared_from_this();

  http::async_read(
      socket_, buffer_, request_,
      [self](beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);
        if (!ec)
          self->process_request();
      });
}
void http_connection::process_request() {
  response_.version(request_.version());
  response_.keep_alive(false);

  switch (request_.method())
  {
    case http::verb::post:
      response_.result(http::status::ok);
      response_.set(http::field::server, "Beast");
      create_response();
      break;

    default:
      // We return responses indicating an error if
      // we do not recognize the request method.
      response_.result(http::status::bad_request);
      response_.set(http::field::content_type, "text/plain");
      response_.body() = "Invalid request-method '"
          + std::string(request_.method_string()) + "'";
      break;
  }

  write_response();
}
void http_connection::create_response() {
  if (request_.target() == "/v1/api/suggest")
  {
    response_.set(http::field::content_type, "application/json");
    response_.body() = sug_.get_json_suggestion(request_.body()).dump(4);
  } else {
    response_.result(http::status::not_found);
    response_.set(http::field::content_type, "text/plain");
    response_.body() = "File not found\r\n";
  }
}
void http_connection::write_response() {
  auto self = shared_from_this();

  response_.content_length(response_.body().size());

  http::async_write(socket_, response_,
                    [self](beast::error_code ec, std::size_t) {
                      self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                      self->deadline_.cancel();
                    });
}
void http_connection::check_deadline() {
  auto self = shared_from_this();

  deadline_.async_wait([self](beast::error_code ec) {
    if (!ec) {
      // Close socket to cancel any outstanding operation.
      self->socket_.close(ec);
    }
  });
}
void http_server(tcp::acceptor& acceptor, tcp::socket& socket, suggestion& sug){
  acceptor.async_accept(socket, [&](beast::error_code ec) {
    if (!ec)
      std::make_shared<http_connection>(std::move(socket), sug)->start();
    http_server(acceptor, socket, sug);
  });
}
