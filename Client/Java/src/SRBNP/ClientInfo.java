package SRBNP;

import java.util.UUID;

class ClientInfo {
	private UUID uuid;

	public UUID getUuid() {
		return uuid;
	}

	public void setUuid(UUID uuid) {
		this.uuid = uuid;
	}
	public void setUuid(String uuid) {
		this.uuid = UUID.fromString(uuid);
	}
	
}
