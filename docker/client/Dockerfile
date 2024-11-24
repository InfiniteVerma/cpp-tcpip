FROM archlinux:latest

# Install runtime dependencies
#RUN pacman -Sy --noconfirm libstdc++6

# Copy your binary
COPY myclient /usr/local/bin/myclient

COPY startup.sh /usr/local/bin/startup.sh
RUN chmod +x /usr/local/bin/startup.sh

CMD ["/usr/local/bin/startup.sh"]
