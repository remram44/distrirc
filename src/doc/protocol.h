/**
 * @page protocol Protocol overview
 *
 * The "extended IRC" protocol that is used between DistrIRC clients and
 * servers (relays) is based on IRC. If one of the party doesn't support the
 * extended commands, the communication should still make sense to regular IRC
 * implementations.
 *
 * The added commands are:
 *
 * @li <tt>DIDENT <protocol version> [identification string]</tt> @n
 *     Sent to the IRC server if it identifies itself as DistrIRC Core. @n
 *     The (optional) identification string is recorded in the logs
 *   (example : "Laptop", "My bouncer on Some-Server", ...).
 *
 * @li <tt>DCONNECT [[[...]PREFIX2~]PREFIX1] <host> <port> [nick] [user] [:<realname>]</tt> @n
 *     Indirectly connects to the specified server. @n
 *     If only one prefix is specified, the server receiving the DCONNECT
 *   connects to the specified address and uses this prefix to identify the new
 *   connection. @n
 *     If multiple prefixes are specified, the server strips the one to the
 *   left and relays the command on the connection it identifies.
 *
 * @li <tt>DDISCONNECT [[...]PREFIX2~]PREFIX1 [:<quit message>]</tt> @n
 *     Disconnects the connection identified by PREFIX1. @n
 *     If only one prefix is specified, the server receiving the DDISCONNECT
 *   disconnects itself from the server specified by the prefix. @n
 *     If multiple prefixes are specified, the server strips the one to the
 *   left and relays the command on the connection it identified.
 *
 * @li <tt>DQUOTE [[...]PREFIX2~]PREFIX1 [:<line>]</tt> @n
 *     Sends a raw command. @n
 *     If only one prefix is specified, the server receiving the DQUOTE sends
 *   the raw line <line> to the server identified by the prefix. @n
 *     If multiple prefixes are specified, the server strips the one to the
 *   left and relays the command on the connection it identified.
 *
 * @li <tt>DSHOWLOG [#][[...]PREFIX2~]PREFIX1~<buffer> <range></tt> @n
 *     Downloads the log for the specified date/position. @n
 *     The server receiving this request can either answer directly if it
 *   possesses the required content (in its logs or cached), or stripping the
 *   prefix to the left and relay the command on the connection it
 *   identified. @n
 *     When the last server is reached, if no content is to be sent back, it
 *   must indicate it back to the querier (TODO : how?). @n
 *     <buffer> may be:
 *     <ul>
 *     <li>"~server": messages from the server (that do not belong in a
 *     particular query or channel buffer)
 *     <li>"~awaylog": private messages and highlights
 *     <li>"~systemlog": system logs of the core
 *     </ul>
 *
 * @li <tt>DLISTLOGS [[...]PREFIX2~]PREFIX1</tt> @n
 *     Downloads the logs list. @n
 *     If only one prefix is specified, the server receiving the DLISTLOGS
 *   sends the list of the logs it has (prefixed name of the buffer, number of
 *   lines and time interval). @n
 *     If multiple prefixes are specified, the server strips the one to the
 *   left and relays the command on the connection it identified.
 *
 * The standard IRC commands are still available, with the added notations:
 *   <ul>
 *   <li>[[...]PREFIX2~]PREFIX1~Nickname and
 *   <li>#[[...]PREFIX2~]PREFIX1~channel
 *   </ul>
 * identifying a user or channel on a specified server.
 * These notations should be recognized by any client.
 *
 * Note that if ~ has to be used, as part of a channel name, user nickname or
 * server tag, it can be escaped as ~~. Don't use it in server tags though,
 * it's just silly.
 */
