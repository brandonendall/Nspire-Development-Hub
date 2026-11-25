"""Sprite groups and rectangle collisions from PythonUltra (MIT)."""

class Sprite:
    def __init__(self, *groups):
        self._groups = []
        if groups:
            self.add(*groups)

    def add(self, *groups):
        for group in groups:
            group.add(self)

    def remove(self, *groups):
        for group in groups:
            group.remove(self)

    def kill(self):
        for group in list(self._groups):
            group.remove(self)

    def alive(self):
        return bool(self._groups)

    def groups(self):
        return list(self._groups)

    def update(self, *args, **kwargs):
        return None


def _flatten_sprites(items):
    for item in items:
        if isinstance(item, (tuple, list)):
            for child in _flatten_sprites(item):
                yield child
        elif isinstance(item, Group):
            for child in item.sprites():
                yield child
        else:
            yield item


class Group:
    def __init__(self, *sprites):
        self._sprites = []
        if sprites:
            self.add(*sprites)

    def __len__(self):
        return len(self._sprites)

    def __iter__(self):
        return iter(self._sprites)

    def __contains__(self, sprite):
        return sprite in self._sprites

    def sprites(self):
        return list(self._sprites)

    def add(self, *sprites):
        for item in _flatten_sprites(sprites):
            if item not in self._sprites:
                self._sprites.append(item)
                if not hasattr(item, "_groups"):
                    item._groups = []
                if self not in item._groups:
                    item._groups.append(self)

    def remove(self, *sprites):
        for item in _flatten_sprites(sprites):
            if item in self._sprites:
                self._sprites.remove(item)
                if hasattr(item, "_groups") and self in item._groups:
                    item._groups.remove(self)

    def has(self, *sprites):
        return all(item in self._sprites for item in _flatten_sprites(sprites))

    def copy(self):
        return self.__class__(self._sprites)

    def empty(self):
        self.remove(list(self._sprites))

    def update(self, *args, **kwargs):
        for item in list(self._sprites):
            item.update(*args, **kwargs)

    def draw(self, surface):
        changed = []
        for item in self._sprites:
            if hasattr(item, "image") and hasattr(item, "rect"):
                changed.append(surface.blit(item.image, item.rect))
        return changed


class RenderPlain(Group):
    pass


RenderUpdates = RenderPlain
OrderedUpdates = RenderPlain


class GroupSingle(Group):
    def __init__(self, sprite=None):
        Group.__init__(self)
        if sprite is not None:
            self.add(sprite)

    def add(self, *sprites):
        values = list(_flatten_sprites(sprites))
        if values:
            self.empty()
            Group.add(self, values[-1])

    @property
    def sprite(self):
        return self._sprites[0] if self._sprites else None

    @sprite.setter
    def sprite(self, value):
        self.empty()
        if value is not None:
            Group.add(self, value)


def collide_rect(left, right):
    return left.rect.colliderect(right.rect)


def collide_rect_ratio(ratio):
    def collision(left, right):
        return left.rect.inflate(left.rect.w * (ratio - 1), left.rect.h * (ratio - 1)).colliderect(
            right.rect.inflate(right.rect.w * (ratio - 1), right.rect.h * (ratio - 1)))
    return collision


def spritecollide(sprite, group, dokill, collided=None):
    test = collided or collide_rect
    hits = [other for other in group if test(sprite, other)]
    if dokill:
        for other in hits:
            other.kill()
    return hits


def spritecollideany(sprite, group, collided=None):
    test = collided or collide_rect
    for other in group:
        if test(sprite, other):
            return other
    return None


def groupcollide(groupa, groupb, dokilla, dokillb, collided=None):
    result = {}
    for left in list(groupa):
        hits = spritecollide(left, groupb, dokillb, collided)
        if hits:
            result[left] = hits
            if dokilla:
                left.kill()
    return result
