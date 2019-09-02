---
title: Purgatory Subscription-related commands
permalink: /Purgatory/Subscription-related_commands/
---

# Subscription-related commands

## Command 0x61: Subscribe Lifecycles

| ubyte  |
| :----: |
| Domain |

Subsequent creation/destruction of an object in *Domain* will trigger an
**Object Creation**/**Object Destruction** response, respectively.

### Response 0x63: Object Creation

| ubyte  | integer  |
| :----: | :------: |
| Domain | DomainId |

Sent after an object *DomainId* was created in a *Domain* the client
previously subscribed to using a **Subscribe Lifecycles** command.

### Response 0x64: Object Destruction

| ubyte  | integer  |
| :----: | :------: |
| Domain | DomainId |

Sent after an object *DomainId* was destroyed in a *Domain* the client
previously subscribed to using a **Subscribe Lifecycles** command.

## Command 0x62: Unsubscribe Lifecycles

| ubyte  |
| :----: |
| Domain |

Cancels a lifecycle subscription previously initiated with a **Subscribe
Lifecycles** command.

## Command 0x65: Subscribe Domain

| ubyte  |     ubyte      |   ubyte    |      ubyte      | ... |   ubyte    |      ubyte      |
| :----: | :------------: | :--------: | :-------------: | :-: | :--------: | :-------------: |
| Domain | Variable Count | Variable 1 | ValueDataType 1 | ... | Variable n | ValueDataType n |

Subsequent update of an object in *Domain* will trigger an **Update
Object** response.

### Response 0x67: Update Object

| ubyte  | integer  | ValueDataType 1 | ... | ValueDataType n |
| :----: | :------: | :-------------: | :-: | :-------------: |
| Domain | DomainId |     Value 1     | ... |     Value n     |

Sent after an object *DomainId* was updated in a *Domain* the client
previously subscribed to using a **Subscribe Domain** command. The
response will contain all variables specified with the last
subscription, using the exact order and type given.

## Command 0x66: Unsubscribe Domain

| ubyte  |
| :----: |
| Domain |

Cancels a domain subscription previously initiated with a **Subscribe
Domain** command.