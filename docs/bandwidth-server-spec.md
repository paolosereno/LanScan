# LanScan Bandwidth Test Server - Specification

## Overview

This document specifies the design and implementation requirements for the LanScan Bandwidth Test Server, a lightweight network service that enables bandwidth measurement between LanScan clients and target devices.

## Purpose

The Bandwidth Test Server is designed to:
- Accept connections from LanScan clients
- Facilitate download and upload speed measurements
- Support both TCP and UDP protocols
- Provide accurate throughput calculations
- Be lightweight and cross-platform

## Architecture

### Server Components

```
┌─────────────────────────────────────┐
│   LanScan Bandwidth Test Server    │
├─────────────────────────────────────┤
│  - TCP Server Thread                │
│  - UDP Server Thread                │
│  - Connection Manager               │
│  - Statistics Calculator            │
│  - Configuration Manager            │
└─────────────────────────────────────┘
```

### Client-Server Communication Flow

```
Client (LanScan)                    Server (Target Device)
     │                                      │
     │──── TCP/UDP Connect ────────────────>│
     │                                      │
     │<─── Connection Accepted ─────────────│
     │                                      │
     │──── Test Parameters ─────────────────>│
     │     (duration, packet size)          │
     │                                      │
     │<─── Ready Signal ────────────────────│
     │                                      │
     │                                      │
     │ ┌──────────────────────────────────┐ │
     │ │   Data Transfer Phase            │ │
     │ │   (duration seconds)             │ │
     │ │                                  │ │
     │ │   Download: Server -> Client    │ │
     │ │   Upload:   Client -> Server    │ │
     │ └──────────────────────────────────┘ │
     │                                      │
     │──── Test Complete ───────────────────>│
     │                                      │
     │<─── Statistics (bytes, time) ────────│
     │                                      │
     │──── Disconnect ──────────────────────>│
     │                                      │
```

## Protocol Specification

### 1. Connection Phase

#### TCP Connection
- Server listens on configurable port (default: 5201)
- Client connects using standard TCP socket
- Server accepts connection and creates dedicated handler thread

#### UDP Connection
- Server listens on configurable port (default: 5201)
- Client sends initial datagram to establish "session"
- Server binds to client's address/port for the session

### 2. Handshake Phase

**Client → Server (Handshake Request)**
```
LANSCAN_BW_TEST\n
VERSION:1.0\n
PROTOCOL:TCP|UDP\n
DIRECTION:DOWNLOAD|UPLOAD\n
DURATION:<seconds>\n
PACKET_SIZE:<bytes>\n
END\n
```

**Server → Client (Handshake Response)**
```
LANSCAN_BW_OK\n
SERVER_VERSION:1.0\n
READY\n
```

### 3. Data Transfer Phase

#### Download Test (Server → Client)
1. Server generates data packets of specified size
2. Server sends packets continuously for specified duration
3. Server counts total bytes sent
4. Client receives and counts bytes

#### Upload Test (Client → Server)
1. Client generates data packets of specified size
2. Client sends packets continuously for specified duration
3. Client counts total bytes sent
4. Server receives and counts bytes

**Data Packet Format:**
```
[SEQUENCE_NUMBER:8 bytes][TIMESTAMP:8 bytes][DATA:N bytes]
```

### 4. Results Phase

**Server → Client (Test Results)**
```
LANSCAN_BW_RESULT\n
BYTES_TRANSFERRED:<bytes>\n
DURATION:<milliseconds>\n
PACKETS_SENT:<count>\n
PACKETS_RECEIVED:<count>\n
PACKET_LOSS:<percentage>\n
END\n
```

## Implementation Requirements

### Server Requirements

#### Functional Requirements
1. **Multi-Protocol Support**
   - Must support TCP and UDP protocols
   - Separate listening threads for each protocol

2. **Concurrent Connections**
   - Support multiple simultaneous clients (max 10 concurrent)
   - Thread pool for handling connections

3. **Configurable Parameters**
   - Listening port (default: 5201)
   - Maximum concurrent connections (default: 10)
   - Buffer size (default: 64 KB)
   - Maximum test duration (default: 60 seconds)

4. **Data Generation**
   - Generate random or zero-filled data packets
   - Support configurable packet sizes (1 KB - 1 MB)

5. **Statistics Tracking**
   - Count bytes transferred
   - Track packet counts
   - Calculate packet loss (UDP only)
   - Measure actual test duration

#### Non-Functional Requirements
1. **Performance**
   - Minimum throughput: 1 Gbps
   - CPU usage: < 25% during active test
   - Memory usage: < 100 MB

2. **Reliability**
   - Graceful handling of client disconnections
   - Timeout handling (30 seconds idle timeout)
   - Error recovery and logging

3. **Security**
   - Optional authentication (future enhancement)
   - Rate limiting per client IP
   - Maximum resource allocation per client

4. **Cross-Platform**
   - Windows 10/11
   - Linux (Ubuntu 20.04+)
   - macOS 11+

### Client (LanScan) Requirements

#### Current Implementation (src/diagnostics/BandwidthTester.cpp)
- Uses Qt's QTcpSocket / QUdpSocket
- Configurable parameters: port, duration, protocol, direction, packet size
- Sends/receives data in chunks
- Calculates bandwidth: (bytes × 8) / duration / 1,000,000

#### Required Modifications
1. Implement protocol handshake
2. Send test parameters to server
3. Parse server responses
4. Handle results phase

## Technology Stack

### Server Options

#### Option 1: C++ with Qt (Recommended)
**Pros:**
- Same stack as LanScan client
- Built-in networking (QTcpServer, QUdpSocket)
- Cross-platform
- Easy integration

**Cons:**
- Qt dependency

**Libraries:**
- Qt 6.x Core, Network

#### Option 2: C++ with Boost.Asio
**Pros:**
- No Qt dependency
- High performance
- Industry standard

**Cons:**
- More complex than Qt
- Additional library dependency

**Libraries:**
- Boost.Asio
- Boost.System

#### Option 3: Python
**Pros:**
- Rapid development
- Easy deployment
- Cross-platform

**Cons:**
- Lower performance
- Not integrated with LanScan codebase

**Libraries:**
- asyncio / socket

### Recommended: Option 1 (C++ with Qt)

## File Structure

```
bandwidth-server/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── src/
│   ├── main.cpp
│   ├── BandwidthServer.h
│   ├── BandwidthServer.cpp
│   ├── TcpHandler.h
│   ├── TcpHandler.cpp
│   ├── UdpHandler.h
│   ├── UdpHandler.cpp
│   ├── ConnectionManager.h
│   ├── ConnectionManager.cpp
│   ├── Protocol.h
│   ├── Protocol.cpp
│   ├── Statistics.h
│   └── Statistics.cpp
├── tests/
│   └── BandwidthServerTest.cpp
└── docs/
    └── protocol.md
```

## Configuration File

**bandwidth-server.conf**
```ini
[Server]
tcp_port=5201
udp_port=5201
max_connections=10
buffer_size=65536
max_test_duration=60

[Logging]
log_level=INFO
log_file=bandwidth-server.log

[Security]
enable_rate_limiting=true
max_requests_per_ip=5
request_window_seconds=60
```

## Command-Line Interface

```bash
# Start server with defaults
./bandwidth-server

# Start server on custom port
./bandwidth-server --port 8080

# Start with specific protocol
./bandwidth-server --tcp-only
./bandwidth-server --udp-only

# Verbose logging
./bandwidth-server --verbose

# Load custom config
./bandwidth-server --config /path/to/config.conf

# Show version
./bandwidth-server --version

# Show help
./bandwidth-server --help
```

## Usage Example

### Server Side
```bash
# Start server on default port
./bandwidth-server
# Output:
# LanScan Bandwidth Test Server v1.0
# TCP Server listening on port 5201
# UDP Server listening on port 5201
# Ready to accept connections...
```

### Client Side (LanScan)
1. Open LanScan
2. Scan network and find device
3. Open Device Detail Dialog
4. Go to Diagnostics tab
5. Click "Test Bandwidth"
6. Configure:
   - Port: 5201
   - Duration: 10 seconds
   - Protocol: TCP
   - Direction: Download
   - Packet Size: 64 KB
7. Click OK
8. View results

## Error Handling

### Server Errors
| Error Code | Description | Action |
|------------|-------------|--------|
| ERR_PORT_IN_USE | Port already in use | Suggest alternative port |
| ERR_MAX_CONNECTIONS | Max connections reached | Reject new connection |
| ERR_INVALID_PROTOCOL | Unknown protocol version | Send error response |
| ERR_TIMEOUT | Client timeout | Close connection |
| ERR_PACKET_SIZE | Invalid packet size | Send error response |

### Client Errors (LanScan)
| Error | Description | User Message |
|-------|-------------|--------------|
| Connection refused | Server not running | "Server not available on target device" |
| Connection timeout | Network issue | "Connection timeout - check firewall" |
| Invalid response | Protocol mismatch | "Incompatible server version" |
| Test aborted | Server error | "Test failed - check server logs" |

## Testing Strategy

### Unit Tests
1. Protocol parsing
2. Data packet generation
3. Statistics calculation
4. Connection management

### Integration Tests
1. TCP download test
2. TCP upload test
3. UDP download test
4. UDP upload test
5. Concurrent connections
6. Timeout handling
7. Error scenarios

### Performance Tests
1. Maximum throughput
2. CPU usage under load
3. Memory usage over time
4. Connection limits

## Security Considerations

### Current Scope
- No authentication (trusted network only)
- Rate limiting by IP address
- Resource limits per connection

### Future Enhancements
- Token-based authentication
- TLS/SSL encryption
- Access control lists (whitelist/blacklist)
- Audit logging

## Deployment

### Binary Distribution
```
bandwidth-server/
├── bandwidth-server.exe (Windows)
├── bandwidth-server (Linux/macOS)
├── bandwidth-server.conf
├── README.md
├── LICENSE
└── docs/
    └── USAGE.md
```

### Installation
```bash
# Linux/macOS
sudo cp bandwidth-server /usr/local/bin/
sudo chmod +x /usr/local/bin/bandwidth-server

# Windows
# Copy to C:\Program Files\LanScan\bandwidth-server\
```

### Systemd Service (Linux)
```ini
[Unit]
Description=LanScan Bandwidth Test Server
After=network.target

[Service]
Type=simple
User=lanscan
ExecStart=/usr/local/bin/bandwidth-server
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

## Roadmap

### Phase 1: Core Implementation (MVP)
- [x] Protocol specification
- [ ] TCP server implementation
- [ ] UDP server implementation
- [ ] Basic statistics
- [ ] Command-line interface

### Phase 2: Client Integration
- [ ] Update LanScan BandwidthTester
- [ ] Protocol handshake implementation
- [ ] Results parsing
- [ ] Error handling

### Phase 3: Testing & Documentation
- [ ] Unit tests
- [ ] Integration tests
- [ ] Performance tests
- [ ] User documentation

### Phase 4: Distribution
- [ ] Binary packaging
- [ ] Installation scripts
- [ ] GitHub release
- [ ] README.md with examples

### Phase 5: Enhancements (Optional)
- [ ] Authentication
- [ ] TLS/SSL encryption
- [ ] Web dashboard
- [ ] Prometheus metrics

## License

Same as LanScan: MIT License

## References

- iperf3 protocol: https://github.com/esnet/iperf
- RFC 6349: Framework for TCP Throughput Testing
- Qt Network Programming: https://doc.qt.io/qt-6/qtnetwork-index.html
