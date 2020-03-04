#pragma once

#include <string>
#include <map>
#include <optional>

class HttpRequest
{
public:
  enum Method
  {
    Invalid,
    Get,
    Post,
    Head,
    Put,
    Delete
  };

  enum Version
  {
    Unknown,
    Http10,
    Http11
  };

  HttpRequest();

  void set_version(Version version);

  Version version() const;

  void set_method(Method method);

  Method method() const;

  void set_path(const char* start, const char* end);

  const std::string& path() const;

  void set_query(const char* start, const char* end);

  const std::string& query() const;

  void add_header(const std::string& field, const std::string& value);

  std::optional<std::string> header(const std::string& field) const;

  const std::map<std::string, std::string>& headers() const;

  void set_body(const char* begin, const char* end);

  const std::string& body() const;

  void swap(HttpRequest& that);

private:
  Method m_method;
  Version m_version;
  std::string m_path;
  std::string m_query;
  std::map<std::string, std::string> m_headers;
  std::string m_body;
};

HttpRequest::HttpRequest()
 : m_method(Invalid)
 , m_version(Unknown)
{

}

void HttpRequest::set_version(Version version)
{
  m_version = version;
}

HttpRequest::Version HttpRequest::version() const
{
  return m_version;
}

void HttpRequest::set_method(Method method)
{
  m_method = method;
}

HttpRequest::Method HttpRequest::method() const
{
  return m_method;
}

void HttpRequest::set_path(const char* start, const char* end)
{
  m_path.assign(start, end);
}

const std::string& HttpRequest::path() const
{
  return m_path;
}

void HttpRequest::set_query(const char* start, const char* end)
{
  m_query.assign(start, end);
}


const std::string& HttpRequest::query() const
{
  return m_query;
}


void HttpRequest::add_header(const std::string& field, const std::string& value)
{
  m_headers[field] = value;
}


std::optional<std::string> HttpRequest::header(const std::string& field) const
{
  auto iter = m_headers.find(field);
  return iter == m_headers.end() ? std::nullopt : make_optional(iter->second);
}

const std::map<std::string, std::string>& HttpRequest::headers() const
{
  return m_headers;
}


void HttpRequest::swap(HttpRequest& that)
{

  //   Method m_method;
  // Version m_version;
  // std::string m_path;
  // std::string m_query;
  // std::map<std::string, std::string> m_headers;
  std::swap(m_method, that.m_method);
  std::swap(m_version, that.m_version);
  m_path.swap(that.m_path);
  m_query.swap(that.m_query);
  m_headers.swap(m_headers);
}