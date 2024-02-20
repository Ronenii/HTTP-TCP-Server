#pragma once
#ifndef HTTPSTATUS
#define HTTPSTATUS

#include <string>


namespace HttpStatus
{
	enum class eCode
	{
		ok =					200,
		created =				201,
		no_content =			204,
		bad_request =			400,
		not_found =				404,
		precondition_failed =	412
	};

	inline bool isSuccessful(int code) { return (code >= 200 && code < 300); } //!< \returns \c true if the given \p code is a successful code.
	inline bool isError(int code) { return (code >= 400); }

	//!< \returns \c true if the given \p code is any type of error code.
	/*! Returns the standard HTTP reason phrase for a HTTP status code.
	* \param code An HTTP status code.
	* \return The standard HTTP reason phrase for the given \p code or an empty \c std::string()
	* if no standard phrase for the given \p code is known.
	*/
	inline std::string reasonPhrase(eCode code)
	{
		switch (code)
		{
		//####### 2xx - Successful #######
		case eCode::ok:						return "OK";
		case eCode::created:				return "Created";
		case eCode::no_content:				return "No Content";


		//####### 4xx - Client Error #######
		case eCode::bad_request:			return "Bad Request";
		case eCode::not_found:				return "Not Found";
		case eCode::precondition_failed:	return "Precondition Failed";

		default: return std::string();
		}
	}

} // namespace HttpStatus


#endif /* HTTPSTATUSCODES_CPP_H_ */
